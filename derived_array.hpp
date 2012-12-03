#pragma once
#include <boost/iterator/indirect_iterator.hpp>
#include <array>
#include <type_traits>

template<typename T, std::size_t MAX_ELEMENTS, std::size_t BLOCK_SIZE, std::size_t ALIGNMENT=alignof(T)>
class DerivedArray {
    struct Block;
    using container_type = std::array<DerivedArray::Block, MAX_ELEMENTS>;
  public:
    using value_type = T;
    using size_type = typename container_type::size_type;
    using difference_type = typename container_type::difference_type;
    using reference = T&;
    using const_reference = T const&;
    using pointer = T*;
    using const_pointer = T const*;
    using iterator = boost::indirect_iterator<typename container_type::iterator>;
    using const_iterator = boost::indirect_iterator<typename container_type::const_iterator>;

  private:
    struct Block {
        using element_type = T;
        typename std::aligned_storage<BLOCK_SIZE, ALIGNMENT>::type storage;

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

        void destroy() {
            data()->~T();
        }

        const_pointer data() const {
            return reinterpret_cast<const_pointer>(&storage);
        }

        pointer data() {
            return reinterpret_cast<pointer>(&storage);
        }

        Block() = default;
        Block(Block const& b) = delete;
        Block(Block&& b) = delete;
        void operator=(Block const&) = delete;
        void operator=(Block&&) = delete;
    };

    container_type elements;
    size_type used = 0;

  public:
    DerivedArray() = default;
    DerivedArray(DerivedArray const&) = delete;
    DerivedArray(DerivedArray&&) = delete;
    void operator=(DerivedArray const&) = delete;
    void operator=(DerivedArray&&) = delete;

    ~DerivedArray() {
        for (std::size_t i = 0; i < used; ++i)
            elements[i].destroy();
    }

    bool empty() const {
        return !used;
    }

    std::size_t max_size() const {
        return MAX_ELEMENTS;
    }

    std::size_t size() const {
        return used;
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

    T& operator[](std::size_t i) {
        return *elements[i];
    }

    T const& operator[](std::size_t i) const {
        return *elements[i];
    }

    template<typename D>
    void push_back(D&& d) {
        using D_Val = typename std::remove_reference<D>::type;
        new (elements[used].data()) D_Val(std::forward<D>(d));
        used += 1;
    }

    void pop_back() {
        used -= 1;
        elements[used].destroy();
    }

    template<typename D>
    void reconstruct(size_type i, D&& d) {
        using D_Val = typename std::remove_reference<D>::type;
        elements[i].destroy();
        new (elements[i].data()) D_Val(std::forward<D>(d));
    }
};
