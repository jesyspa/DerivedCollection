#pragma once
#include <array>
#include <type_traits>

template<typename T, std::size_t MAX_ELEMENTS, std::size_t BLOCK_SIZE, std::size_t ALIGNMENT=alignof(T)>
class DerivedArray {
    struct Block {
        bool active = false;
        typename std::aligned_storage<BLOCK_SIZE, ALIGNMENT>::type storage;

        ~Block() {
            destroy();
        }

        T const& operator*() const {
            return *reinterpret_cast<T const*>(&storage);
        }

        T& operator*() {
            return *reinterpret_cast<T*>(&storage);
        }

        T const* operator->() const {
            return &**this;
        }

        T* operator->() {
            return &**this;
        }

        void destroy() {
            if (!active)
                return;
            // Needs to be in this order to prevent double destruction if the
            // destructor throws.
            active = false;
            (*this)->~T();
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
    void push_back(D const& d) {
        new (&elements[used].storage) D(d);
        elements[used].active = true;
        used += 1;
    }

    template<typename D>
    void push_back(D&& d) {
        new (&elements[used].storage) D(std::move(d));
        elements[used].active = true;
        used += 1;
    }

    void pop_back() {
        used -= 1;
        elements[used].destroy();
    }
};

