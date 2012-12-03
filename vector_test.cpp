#include "derived_vector.hpp"
#include "test_classes.hpp"
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(vector_construct) {
    DerivedVector<Base, sizeof(Base)> vector;
    BOOST_CHECK(vector.empty());
}

BOOST_AUTO_TEST_CASE(vector_push_back) {
    DerivedVector<Base, sizeof(Var)> vector;
    BOOST_CHECK_EQUAL(vector.size(), 0);
    vector.push_back(Five());
    BOOST_CHECK_EQUAL(vector.size(), 1);
    vector.push_back(Five());
    BOOST_CHECK_EQUAL(vector.size(), 2);
}

BOOST_AUTO_TEST_CASE(vector_correct_virtual) {
    DerivedVector<Base, sizeof(Var)> vector;
    vector.push_back(Var(3));
    vector.push_back(Five());
    BOOST_CHECK_EQUAL(vector[0].get_num(), 3);
    BOOST_CHECK_EQUAL(vector[1].get_num(), 5);
}

BOOST_AUTO_TEST_CASE(vector_pop_back) {
    DerivedVector<Base, sizeof(Var)> vector;
    BOOST_CHECK_EQUAL(vector.size(), 0);
    vector.push_back(Five());
    BOOST_CHECK_EQUAL(vector.size(), 1);
    vector.push_back(Five());
    BOOST_CHECK_EQUAL(vector.size(), 2);
    vector.pop_back();
    BOOST_CHECK_EQUAL(vector.size(), 1);
    vector.pop_back();
    BOOST_CHECK_EQUAL(vector.size(), 0);
}

BOOST_AUTO_TEST_CASE(vector_ensure_destructors_run) {
    DerivedVector<Base, sizeof(Dynamic)> vector;
    auto ptr = std::make_shared<int>();
    vector.push_back(Dynamic(ptr));
    BOOST_CHECK_EQUAL(ptr.use_count(), 2);
    vector.push_back(Dynamic(ptr));
    BOOST_CHECK_EQUAL(ptr.use_count(), 3);
    vector.pop_back();
    vector.pop_back();
    BOOST_CHECK_EQUAL(ptr.use_count(), 1);
}

BOOST_AUTO_TEST_CASE(vector_movement) {
    DerivedVector<Base, sizeof(Dynamic)> vector;
    auto ptr = std::make_shared<int>(3);
    vector.push_back(Dynamic(ptr));
    DerivedVector<Base, sizeof(Dynamic)> moved(std::move(vector));
    BOOST_CHECK_EQUAL(moved.size(), 1);
    BOOST_CHECK_EQUAL(ptr.use_count(), 2);
    BOOST_CHECK_EQUAL(moved[0].get_num(), *ptr);
}

BOOST_AUTO_TEST_CASE(vector_iteration) {
    DerivedVector<Base, sizeof(Dynamic)> vector;
    vector.push_back(Var(5));
    vector.push_back(Five());
    for (auto& e : vector)
        BOOST_CHECK_EQUAL(e.get_num(), 5);
}

BOOST_AUTO_TEST_CASE(vector_replace) {
    DerivedVector<Base, sizeof(Var)> vector;
    vector.push_back(Five());
    vector.reconstruct(0, Var(3));
    BOOST_CHECK_EQUAL(vector.size(), 1);
    BOOST_CHECK_EQUAL(vector[0].get_num(), 3);
}
