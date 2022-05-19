#include "two_dimensional_array.h"

#include <iostream>

#include "gtest/gtest.h"

template <typename T>
std::ostream& operator<<(std::ostream& stream,
                         const Two_dimensional_array<T>& array) {
    stream << std::endl;
    for (auto r = array.cbegin(); r != array.cend(); ++r) {
        for (const auto* e = r->begin(); e != r->end(); ++e) {
            stream.width(3);
            stream << *e << " ";
        }
        stream << std::endl;
    }
    return stream;
}

template <typename T>
std::string to_string(const T& t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
}

TEST(Two_dimensional_array_test, base) {
    {
        Two_dimensional_array<int> array(5, 5);

        int i = -1;
        for (auto row : array)
            for (auto& el : row) el = ++i;

        const char* expected = R"(
  0   1   2   3   4 
  5   6   7   8   9 
 10  11  12  13  14 
 15  16  17  18  19 
 20  21  22  23  24 
)";

        ASSERT_EQ(expected, to_string(array));

        auto array_copy = array;

        ASSERT_EQ(0, array.get(0, 0));
        ASSERT_EQ(22, array.get(4, 2));
        ASSERT_EQ(16, array[3][1]);

        ASSERT_EQ(expected, to_string(array_copy));

        array_copy = array;
        ASSERT_EQ(expected, to_string(array_copy));

        array[3][1] = 99;
        ASSERT_EQ(99, array[3][1]);
    }
    {
        Two_dimensional_array<int> array(5, 5);
        array.fill(0);

        ASSERT_EQ(R"(
  0   0   0   0   0 
  0   0   0   0   0 
  0   0   0   0   0 
  0   0   0   0   0 
  0   0   0   0   0 
)",
                  to_string(array));
    }
}
