#include "gtest/gtest.h"

#include "forward_list.h"
#include "pair.h"

#include <sstream>

TEST(Forward_list_test, test_0) {
    using list = Forward_list<int>;

    auto to_string = [](const list& l) {
        std::stringstream ss;
        for (auto it = l.cbegin(); it != l.cend(); ++it) ss << (*it) << " ";
        return ss.str();
    };

    list l;
    l.push_back(3);
    l.push_back(7);
    l.push_back(9);
    l.push_back(2);

    ASSERT_EQ("3 7 9 2 ", to_string(l));

    auto it = l.begin(); *it = 4;
    ++it; *it = 5;
    ++it; *it = 7;
    ++it; *it = 8;
    ++it; ASSERT_TRUE(it == l.end());

    ASSERT_EQ("4 5 7 8 ", to_string(l));

    std::stringstream ss;
    for (auto i : l) ss << i << " ";
    ASSERT_EQ("4 5 7 8 ", ss.str());
}

TEST(Forward_list_test, test_1) {
    Forward_list<Pair<int, int>> list;
    list.emplace_back(1, 2);
    ASSERT_EQ(Pair(1, 2), *list.begin());
}
