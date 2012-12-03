#pragma once
#include <boost/iterator/indirect_iterator.hpp>
#include <type_traits>
#include <vector>

template<typename T, std::size_t BLOCK_SIZE, std::size_t ALIGNMENT=alignof(T)>
class DerivedVector {
    struct Block;
  public:
    using value_type = T;
    using reference = T&;
    using const_reference = T const&;
    using pointer = T*;
    using const_pointer = T const*;

  private:
    struct TypeInfo {
        using deleter_t = void(*)(void*);
        deleter_t deleter;
        using mover_t = void(*)(Block&, void*);
        mover_t mover;
    };

    template<typename D>
    struct TypeInfoImpl {
        static_assert(std::is_base_of<T, D>::value, "Class is not derived!");
        static_assert(sizeof(D) <= BLOCK_SIZE, "Derived class too big");
        static_assert(ALIGNMENT % alignof(D) == 0, "Derived class has incompatible alignment");

        static void deleter(void* p) {
            static_cast<D*>(p)->~D();
        }

        static void mover(Block& p, void* o) {
            new (p.data()) D(std::move(*static_cast<D*>(o)));
        }

        static TypeInfo* get() {
            static TypeInfo info{deleter, mover};
            return &info;
        }
    };

    struct Block {
        using element_type = T;
        TypeInfo* info = nullptr;
        typename std::aligned_storage<BLOCK_SIZE, ALIGNMENT>::type storage;

        Block() = default;

        template<typename D>
        Block(D&& d) {
            construct(std::forward<D>(d));
        }

        Block(Block&& b) : info(b.info) {
            if (info)
                info->mover(*this, b.data());
        }

        Block& operator=(Block&& b) {
            if (this == &b)
                return *this;
            destroy();
            info = b.info;
            info->mover(*this, b.data());
            return *this;
        }

        Block(Block const& b) = delete;
        void operator=(Block const&) = delete;

        template<typename D>
        void construct(D&& d) {
            using D_Val = typename std::remove_reference<D>::type;
            new (data()) D_Val(std::forward<D>(d));
            info = TypeInfoImpl<D_Val>::get();
        }

        ~Block() {
            destroy();
        }

        void destroy() {
            if (!info)
                return;
            auto deleter = info->deleter;
            info = nullptr;
            deleter(data());
        }

        const_reference operator*() const {
            return *data();
        }

        reference operator*() {
            return *data();
        }

        const_pointer operator->() const {
            return data();
        }

        pointer operator->() {
            return data();
        }

        const_pointer data() const {
            return reinterpret_cast<const_pointer>(&storage);
        }

        pointer data() {
            return reinterpret_cast<pointer>(&storage);
        }
    };

    using container_type = std::vector<Block>;
    container_type elements;

  public:
    using size_type = typename container_type::size_type;
    using difference_type = typename container_type::difference_type;
    using iterator = boost::indirect_iterator<typename container_type::iterator>;
    using const_iterator = boost::indirect_iterator<typename container_type::const_iterator>;

    DerivedVector() = default;
    DerivedVector(DerivedVector&&) = default;
    DerivedVector& operator=(DerivedVector&&) = default;
    DerivedVector(DerivedVector const&) = delete;
    void operator=(DerivedVector const&) = delete;

    bool empty() const {
        return elements.empty();
    }

    size_type size() const {
        return elements.size();
    }

    size_type max_size() const {
        return elements.max_size();
    }

    reference operator[](size_type i) {
        return *elements[i];
    }

    const_reference operator[](size_type i) const {
        return *elements[i];
    }

    const_iterator begin() const {
        return elements.begin();
    }

    iterator begin() {
        return elements.begin();
    }

    const_iterator cbegin() const {
        return elements.cbegin();
    }

    const_iterator end() const {
        return elements.end();
    }

    iterator end() {
        return elements.end();
    }

    const_iterator cend() const {
        return elements.cend();
    }

    template<typename D>
    void push_back(D&& d) {
        elements.emplace_back(d);
    }

    void pop_back() {
        elements.pop_back();
    }

    template<typename D>
    void reconstruct(size_type i, D&& d) {
        elements[i].destroy();
        elements[i].construct(std::forward<D>(d));
    }
};
