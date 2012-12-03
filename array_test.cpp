#include "derived_array.hpp"
#include <memory>
#include <iostream>

struct Base {
    virtual int get_num() const = 0;
    virtual ~Base() = default;
};

struct Five : Base {
    int get_num() const {
        return 5;
    }
};

struct Var : Base {
    Var(int x) : var(x) {}

    int get_num() const {
        return var;
    }

  private:
    int var;
};

struct Dynamic : Base {
    Dynamic(int x) : ptr(new int(x)) {}

    int get_num() const {
        return *ptr;
    }

  private:

    std::unique_ptr<int> ptr;
};

int main() {
    DerivedArray<Base, 5, sizeof(Var)> array;
    array.push_back(Var(3));
    array.push_back(Five());
    array.push_back(Var(7));
    array.push_back(Dynamic(9));

    for (std::size_t i = 0; i < array.size(); ++i)
        std::cout << array[i].get_num() << '\n';

    array[1] = Dynamic(11);
    array[3] = Five();
    array.pop_back();
    array.pop_back();
}
