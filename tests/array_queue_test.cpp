#include "gtest/gtest.h"

#include "array_queue.h"

TEST(Array_queue_test, base) {
    Array_queue<int> q(10);
    ASSERT_TRUE(q.empty());
    
    for (int i = 10; i < 14; ++i)
        q.push(i);

    std::stringstream s;
    for (int i = 0; i < 3; ++i)
        s << q.pop() << " ";
    ASSERT_EQ("10 11 12 ", s.str());
    ASSERT_FALSE(q.empty());

    ASSERT_EQ(13, q.pop());
    ASSERT_TRUE(q.empty());

    for (int i = 14; i < 24; ++i)
        q.push(i);
    s = std::stringstream();
    for (int i = 0; i < 10; ++i)
        s << q.pop() << " ";
    ASSERT_EQ("14 15 16 17 18 19 20 21 22 23 ", s.str());
    ASSERT_TRUE(q.empty());
}

TEST(Array_queue_test, emplace) {
    Array_queue<std::pair<int, int>> q(10);
    ASSERT_TRUE(q.empty());
    for (int i = 0; i < 10; ++i)
        q.emplace(i, i);
    ASSERT_FALSE(q.empty());

    std::stringstream s;
    for (int i = 0; i < 10; ++i) {
        auto p = q.pop();
        s << p.first << ":" << p.second << " ";
    }
    ASSERT_EQ("0:0 1:1 2:2 3:3 4:4 5:5 6:6 7:7 8:8 9:9 ", s.str());
    ASSERT_TRUE(q.empty());
}


