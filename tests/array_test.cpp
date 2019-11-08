#include "gtest/gtest.h"

#include "array.h"

#include <iostream>

TEST(Array_test, test_0) {
    auto to_string = [](Array<bool>& a) {
        std::stringstream ss;
        for (auto& i : a)
            ss << i << " ";
        return ss.str();
    };

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
        for (auto it = a.cbegin(); it != a.cend(); ++it)
            ss << *it << " ";
        return ss.str();
    };
    ASSERT_EQ("0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 ", to_string_const(a));

    {
        auto it = a.begin();
        for (int i = 0; i < 11; ++i, ++it) {
            *it = true;
        }
        ASSERT_EQ("1 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 1 ", to_string_const(a));
    }
}

