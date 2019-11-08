#include "gtest/gtest.h"

#include "array.h"

#include <iostream>

TEST(Array_test, test_0) {

    // auto to_string = [](const auto& a) {
    //     std::stringstream ss;
    //     for (auto& i : a)  todo add iterators
    //         ss << i << " ";
    //     return ss.str();
    // };

    Array<bool> a(20);
    a[0] = true;
    a[10] = true;
    a[13] = true;
    a[19] = true;

    // std::cout << to_string(a);

}

