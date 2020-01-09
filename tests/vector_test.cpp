#include "gtest/gtest.h"

#include "vector.h"

#include <sstream>

TEST(Vector_test, test_1) {
    auto to_string = [](const Vector<int>& vector) {
        std::stringstream ss;
        ss << vector;
        return ss.str();
    };
    Vector<int> v(5);
    v[1] = 1;
    v[2] = 2;
    ASSERT_EQ(v.size(), 5);
    ASSERT_EQ(to_string(v), "[0, 1, 2, 0, 0]");
    v[3] = 3;
    v[4] = 4;
    for (int i = 5; i < 20; ++i)
        v.push_back(i);
    ASSERT_EQ(v.size(), 20);
    ASSERT_EQ(to_string(v), "[0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19]");

    for (int i = 0; i < 20; ++i)
        ASSERT_EQ(v[i], i);

    Vector<int> v2{1, 2, 3};
    ASSERT_EQ(to_string(v2), "[1, 2, 3]");

    Vector<int> v_copy = v2;
    ASSERT_EQ(to_string(v_copy), "[1, 2, 3]");
    v_copy[0] = 3;
    ASSERT_EQ(to_string(v2), "[1, 2, 3]");
    ASSERT_EQ(to_string(v_copy), "[3, 2, 3]");

    v_copy = v2;
    ASSERT_EQ(to_string(v_copy), "[1, 2, 3]");
    v_copy[0] = 3;
    ASSERT_EQ(to_string(v2), "[1, 2, 3]");
    ASSERT_EQ(to_string(v_copy), "[3, 2, 3]");

    v_copy = v2;
    auto v_move = std::move(v_copy);
    ASSERT_EQ(to_string(v_move), "[1, 2, 3]");
    ASSERT_EQ(to_string(v_copy), "[]");

    v_copy = v2;
    v_move[0] = 2;
    v_move = std::move(v_copy);
    ASSERT_EQ(to_string(v_move), "[1, 2, 3]");
    ASSERT_EQ(to_string(v_copy), "[2, 2, 3]");

    v = {};
    ASSERT_EQ(to_string(v), "[]");
    ASSERT_EQ(v.size(), 0);
    v.push_back(1);
    ASSERT_EQ(to_string(v), "[1]");
    ASSERT_EQ(v.size(), 1);
}

