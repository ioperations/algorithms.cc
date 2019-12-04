#include "gtest/gtest.h"

#include "heap.h"

TEST(Heap_test, test_1) {
    auto test_sort = [](Array<int> array, Array<int> expected) {
        heap_sort(array.begin(), array.end());
        ASSERT_EQ(array, expected);
    };

    test_sort({11, 5, 1, 3, 9, 2}, {1, 2, 3, 5, 9, 11});
    test_sort({1, 3, 2}, {1, 2, 3});

    Heap<int> heap;
    for (auto i : {11, 5, 1, 3, 9, 2})
        heap.push(i);

    auto size = heap.size();
    Array<int> array(size);
    for (auto el = array.end() - 1; el != array.begin() - 1; --el)
        *el = heap.pop();
    ASSERT_EQ(array, Array<int>({1, 2, 3, 5, 9, 11}));
}
