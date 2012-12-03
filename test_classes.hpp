#pragma once
#include <memory>

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
    Dynamic(std::shared_ptr<int> x) : ptr(x) {}

    int get_num() const {
        return *ptr;
    }

  private:
    std::shared_ptr<int> ptr;
};
