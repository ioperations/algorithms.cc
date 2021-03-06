#include "array.h"

#include <iostream>

#include "gtest/gtest.h"
#include "test_utils.h"

template <typename A>
auto to_string(A& a) {
    std::stringstream ss;
    for (auto& i : a) ss << i << " ";
    return ss.str();
}

TEST(Array_test, base) {
    Array<int> a(15);
    const char* expected = "1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 ";
    for (size_t i = 0; i < a.size(); ++i) a[i] = i + 1;
    ASSERT_EQ(expected, to_string(a));
    auto a_copy = a;
    ASSERT_EQ(expected, to_string(a_copy));
    a_copy = a;
    ASSERT_EQ(expected, to_string(a_copy));
}

TEST(Array_test, bool_specialization) {
    Array<bool> a(20);
    a[0] = true;
    a[10] = true;
    a[13] = true;
    a[19] = true;

    ASSERT_EQ("1 0 0 0 0 0 0 0 0 0 1 0 0 1 0 0 0 0 0 1 ", to_string(a));
    a[0] = false;
    a[9] = false;
    a[10] = false;
    a[13] = false;
    ASSERT_EQ("0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 ", to_string(a));

    auto to_string_const = [](const Array<bool>& a) {
        std::stringstream ss;
        for (auto it = a.cbegin(); it != a.cend(); ++it) ss << *it << " ";
        return ss.str();
    };
    ASSERT_EQ("0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 ", to_string_const(a));

    {
        auto it = a.begin();
        for (int i = 0; i < 11; ++i, ++it) {
            *it = true;
        }
        ASSERT_EQ("1 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 1 ",
                  to_string_const(a));
    }

    a = Array<bool>::build_array(false, true, false, true);
    ASSERT_EQ("0 1 0 1 ", to_string_const(a));

    a = Array<bool>(5, true);
    ASSERT_EQ("[1, 1, 1, 1, 1]", stringify(a));
    a = Array<bool>(5, true);
    ASSERT_EQ("[1, 1, 1, 1, 1]", stringify(a));
    a = Array<bool>(5, false);
    ASSERT_EQ("[0, 0, 0, 0, 0]", stringify(a));
    a = Array<bool>(5, false);
    ASSERT_EQ("[0, 0, 0, 0, 0]", stringify(a));

    auto a2 = a;
    ASSERT_EQ("[0, 0, 0, 0, 0]", stringify(a2));
    a[0] = true;
    ASSERT_EQ("[1, 0, 0, 0, 0]", stringify(a));
    ASSERT_EQ("[0, 0, 0, 0, 0]", stringify(a2));
}

TEST(Array_test, initializer_list) {
    Array<int> a{1, 2, 3, 4, 5};
    ASSERT_EQ("1 2 3 4 5 ", to_string(a));
}

TEST(Array_test, reverse_iterator) {
    Array<int> a{1, 2, 3, 4, 5};
    std::stringstream ss;
    for (auto it = a.crbegin(); it != a.crend(); ++it) ss << *it << " ";
    ASSERT_EQ("5 4 3 2 1 ", ss.str());
    ss = std::stringstream();
    for (auto it = a.rbegin(); it != a.rend(); ++it) ss << *it << " ";
    ASSERT_EQ("5 4 3 2 1 ", ss.str());
}
