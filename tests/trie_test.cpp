#include "trie.h"

#include <iostream>

#include "gtest/gtest.h"

TEST(t0, t1) {
    Trie<int> prefix_tree;
    prefix_tree.Insert(std::vector<int>{1, 2, 3, 4, 5, 6});

    bool ret = prefix_tree.StartWith(std::vector<int>{1, 2, 3});
    EXPECT_EQ(ret, true);
    ret = prefix_tree.StartWith(std::vector<int>{1, 2, 3, 4, 5});
    EXPECT_EQ(ret, true);
    prefix_tree.Insert(std::vector<int>{5, 4, 3, 2, 1});

    ret = prefix_tree.StartWith(std::vector<int>{5, 4, 3});
    EXPECT_EQ(ret, true);

    ret = prefix_tree.Search(std::vector<int>{5, 4, 3, 2, 1});
    EXPECT_EQ(ret, true);
}

TEST(t0, t2) {
    Trie<std::string> prefix_tree;
    prefix_tree.Insert(std::vector<std::string>{"1", "2", "3", "4", "5", "6"});

    bool ret = prefix_tree.StartWith(std::vector<std::string>{"1", "2", "3"});
    EXPECT_EQ(ret, true);
    ret = prefix_tree.StartWith(
        std::vector<std::string>{"1", "2", "3", "4", "5"});
    EXPECT_EQ(ret, true);
    prefix_tree.Insert(std::vector<std::string>{"5", "4", "3", "2", "1"});

    ret = prefix_tree.StartWith(std::vector<std::string>{"5", "4", "3"});
    EXPECT_EQ(ret, true);

    ret = prefix_tree.Search(std::vector<std::string>{"5", "4", "3", "2", "1"});
    EXPECT_EQ(ret, true);
}

TEST(t0, t3) {
    Trie prefix_tree;
    prefix_tree.Insert(std::vector<std::string>{"1", "2", "3", "4", "5", "6"});

    bool ret = prefix_tree.StartWith(std::vector<std::string>{"1", "2", "3"});
    EXPECT_EQ(ret, true);
    ret = prefix_tree.StartWith(
        std::vector<std::string>{"1", "2", "3", "4", "5"});
    EXPECT_EQ(ret, true);
    prefix_tree.Insert(std::vector<std::string>{"5", "4", "3", "2", "1"});

    ret = prefix_tree.StartWith(std::vector<std::string>{"5", "4", "3"});
    EXPECT_EQ(ret, true);

    ret = prefix_tree.Search(std::vector<std::string>{"5", "4", "3", "2", "1"});
    EXPECT_EQ(ret, true);
}

TEST(t0, t4) {
    //    ["Trie", "insert", "search", "search", "startsWith", "insert",
    //    "search"]
    //    [[], ["apple"], ["apple"], ["app"], ["app"], ["app"], ["app"]]
    //    Output [null, null, true, false, true, null, true]

    Trie trie;
    trie.Insert("apple");
    auto ret = trie.Search("apple");  // return True
    EXPECT_EQ(ret, true);
    ret = trie.Search("app");  // return False
    EXPECT_EQ(ret, false);
    ret = trie.StartWith("app");  // return True
    EXPECT_EQ(ret, true);
    trie.Insert("app");
    ret = trie.Search("app");  // return True
    EXPECT_EQ(ret, true);
}

