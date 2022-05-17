#include "forward_list.h"

#include <sstream>

#include "gtest/gtest.h"
#include "pair.h"

template <typename T>
auto to_string(const Forward_list<T>& l) {
    std::stringstream ss;
    for (auto it = l.cbegin(); it != l.cend(); ++it) ss << (*it) << " ";
    return ss.str();
}

TEST(Forward_list_test, base) {
    using list = Forward_list<int>;
    list l;
    auto fill_list = [&l]() {
        l.push_back(3);
        l.push_back(7);
        l.push_back(9);
        l.push_back(2);
    };
    fill_list();

    l.clear();
    ASSERT_TRUE(l.empty());
    fill_list();
    ASSERT_FALSE(l.empty());

    ASSERT_EQ("3 7 9 2 ", to_string(l));

    auto it = l.begin();
    *it = 4;
    ++it;
    *it = 5;
    ++it;
    *it = 7;
    ++it;
    *it = 8;
    ++it;
    ASSERT_TRUE(it == l.end());

    ASSERT_EQ("4 5 7 8 ", to_string(l));

    std::stringstream ss;
    for (auto i : l) ss << i << " ";
    ASSERT_EQ("4 5 7 8 ", ss.str());
}

TEST(Forward_list_test, pair) {
    Forward_list<Pair<int, int>> list;
    list.emplace_back(1, 2);
    ASSERT_EQ(Pair(1, 2), *list.begin());
}

TEST(Forward_list_test, pop_front) {
    Forward_list<int> list;
    for (int i = 0; i < 10; ++i) list.push_back(i);

    Forward_list<int> list_2;
    while (!list.empty()) list_2.push_back(list.pop_front());

    ASSERT_EQ("0 1 2 3 4 5 6 7 8 9 ", to_string(list_2));

    list.push_back(10);
    list.push_back(11);
    list.push_back(12);
    ASSERT_EQ("10 11 12 ", to_string(list));
}

TEST(Forward_list_test, merge_sort) {
    Forward_list<int> list{5, 3, 1, 4, 2};
    ASSERT_EQ("5 3 1 4 2 ", to_string(list));
    list.merge_sort();
    ASSERT_EQ("1 2 3 4 5 ", to_string(list));

    list = {1};
    list.merge_sort();
    ASSERT_EQ("1 ", to_string(list));

    list = {};
    list.merge_sort();
    ASSERT_EQ("", to_string(list));
}

TEST(Forward_list_test, remove_element) {
    Forward_list<int> list{5, 3};
    ASSERT_TRUE(list.remove_first_if([](int value) { return value == 5; }));
    ASSERT_EQ("3 ", to_string(list));
    ASSERT_FALSE(list.remove_first_if([](int value) { return value == 2; }));
    ASSERT_EQ("3 ", to_string(list));
    ASSERT_TRUE(list.remove_first_if([](int value) { return value == 3; }));
    ASSERT_EQ(true, list.empty());
    ASSERT_FALSE(list.remove_first_if([](int value) { return value == 3; }));

    list = {5, 2, 3};
    ASSERT_TRUE(list.remove_first_if([](int value) { return value == 2; }));
    ASSERT_EQ("5 3 ", to_string(list));
    ASSERT_TRUE(list.remove_first_if([](int value) { return value == 3; }));
    ASSERT_EQ("5 ", to_string(list));
    ASSERT_TRUE(list.remove_first_if([](int value) { return value == 5; }));
    ASSERT_EQ(true, list.empty());
    ASSERT_EQ("", to_string(list));
}
