#include "stack.h"

#include "gtest/gtest.h"

TEST(Stack_test, test_0) {
    Stack<int> stack;
    ASSERT_TRUE(stack.empty());

    stack.push(1);
    ASSERT_FALSE(stack.empty());
    ASSERT_EQ(1, stack.pop());
    ASSERT_TRUE(stack.empty());

    stack.push(1);
    stack.push(2);
    ASSERT_FALSE(stack.empty());
    ASSERT_EQ(2, stack.pop());
    ASSERT_EQ(1, stack.pop());
    ASSERT_TRUE(stack.empty());
}
