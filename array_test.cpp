#include "derived_array.hpp"
#define BOOST_TEST_MODULE
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
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

BOOST_AUTO_TEST_CASE(construct) {
    DerivedArray<Base, 1, sizeof(Base)> array;
    BOOST_CHECK_EQUAL(array.max_size(), 1);
    BOOST_CHECK(array.empty());
}

BOOST_AUTO_TEST_CASE(construct_empty) {
    DerivedArray<Base, 0, sizeof(Base)> array;
    BOOST_CHECK_EQUAL(array.max_size(), 0);
    BOOST_CHECK(array.empty());
}

BOOST_AUTO_TEST_CASE(push_back) {
    DerivedArray<Base, 3, sizeof(Var)> array;
    BOOST_CHECK_EQUAL(array.size(), 0);
    array.push_back(Five());
    BOOST_CHECK_EQUAL(array.size(), 1);
    array.push_back(Five());
    BOOST_CHECK_EQUAL(array.size(), 2);
}

BOOST_AUTO_TEST_CASE(correct_virtual) {
    DerivedArray<Base, 2, sizeof(Var)> array;
    array.push_back(Var(3));
    array.push_back(Five());
    BOOST_CHECK_EQUAL(array[0].get_num(), 3);
    BOOST_CHECK_EQUAL(array[1].get_num(), 5);
}

BOOST_AUTO_TEST_CASE(pop_back) {
    DerivedArray<Base, 3, sizeof(Var)> array;
    BOOST_CHECK_EQUAL(array.size(), 0);
    array.push_back(Five());
    BOOST_CHECK_EQUAL(array.size(), 1);
    array.push_back(Five());
    BOOST_CHECK_EQUAL(array.size(), 2);
    array.pop_back();
    BOOST_CHECK_EQUAL(array.size(), 1);
    array.pop_back();
    BOOST_CHECK_EQUAL(array.size(), 0);
}

BOOST_AUTO_TEST_CASE(ensure_destructors_run) {
    DerivedArray<Base, 3, sizeof(Dynamic)> array;
    auto ptr = std::make_shared<int>();
    array.push_back(Dynamic(ptr));
    BOOST_CHECK_EQUAL(ptr.use_count(), 2);
    array.push_back(Dynamic(ptr));
    BOOST_CHECK_EQUAL(ptr.use_count(), 3);
    array.pop_back();
    array.pop_back();
    BOOST_CHECK_EQUAL(ptr.use_count(), 1);
}
