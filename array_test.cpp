#include "derived_array.hpp"
#include "test_classes.hpp"
#define BOOST_TEST_MODULE
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(array_construct) {
    DerivedArray<Base, 1, sizeof(Base)> array;
    BOOST_CHECK_EQUAL(array.max_size(), 1);
    BOOST_CHECK(array.empty());
}

BOOST_AUTO_TEST_CASE(array_construct_empty) {
    DerivedArray<Base, 0, sizeof(Base)> array;
    BOOST_CHECK_EQUAL(array.max_size(), 0);
    BOOST_CHECK(array.empty());
}

BOOST_AUTO_TEST_CASE(array_push_back) {
    DerivedArray<Base, 3, sizeof(Var)> array;
    BOOST_CHECK_EQUAL(array.size(), 0);
    array.push_back(Five());
    BOOST_CHECK_EQUAL(array.size(), 1);
    array.push_back(Five());
    BOOST_CHECK_EQUAL(array.size(), 2);
    Five f;
    array.push_back(f);
    BOOST_CHECK_EQUAL(array.size(), 3);
    BOOST_CHECK_EQUAL(array[2].get_num(), 5);
}

BOOST_AUTO_TEST_CASE(array_correct_virtual) {
    DerivedArray<Base, 2, sizeof(Var)> array;
    array.push_back(Var(3));
    array.push_back(Five());
    BOOST_CHECK_EQUAL(array[0].get_num(), 3);
    BOOST_CHECK_EQUAL(array[1].get_num(), 5);
}

BOOST_AUTO_TEST_CASE(array_pop_back) {
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

BOOST_AUTO_TEST_CASE(array_ensure_destructors_run) {
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

BOOST_AUTO_TEST_CASE(array_reconstruct) {
    DerivedArray<Base, 3, sizeof(Dynamic)> array;
    auto ptr = std::make_shared<int>();
    array.push_back(Dynamic(ptr));
    array.reconstruct(0, Var(1));
    BOOST_CHECK_EQUAL(ptr.use_count(), 1);
    BOOST_CHECK_EQUAL(array[0].get_num(), 1);
}

BOOST_AUTO_TEST_CASE(array_iterate) {
    DerivedArray<Base, 3, sizeof(Dynamic)> array;
    auto ptr = std::make_shared<int>(5);
    array.push_back(Dynamic(ptr));
    array.push_back(Five());
    array.push_back(Var(5));
    for (auto& e : array)
        BOOST_CHECK_EQUAL(e.get_num(), 5);
}
