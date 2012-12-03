#pragma once
#include <array>
#include <type_traits>

template<typename T, std::size_t MAX_ELEMENTS, std::size_t BLOCK_SIZE, std::size_t ALIGNMENT=alignof(T)>
class DerivedArray {
  public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = T const&;
    using pointer = T*;
    using const_pointer = T const*;

  private:
    struct Block {
        bool active = false;
        typename std::aligned_storage<BLOCK_SIZE, ALIGNMENT>::type storage;

        ~Block() {
            destroy();
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

        void destroy() {
            if (!active)
                return;
            // Needs to be in this order to prevent double destruction if the
            // destructor throws.
            active = false;
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

    std::array<Block, MAX_ELEMENTS> elements;
    std::size_t used = 0;

  public:
    DerivedArray() = default;
    DerivedArray(DerivedArray const&) = delete;
    DerivedArray(DerivedArray&&) = delete;
    void operator=(DerivedArray const&) = delete;
    void operator=(DerivedArray&&) = delete;

    bool empty() const {
        return !used;
    }

    std::size_t max_size() const {
        return MAX_ELEMENTS;
    }

    std::size_t size() const {
        return used;
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
        elements[used].active = true;
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
        elements[i].active = true;
    }
};
