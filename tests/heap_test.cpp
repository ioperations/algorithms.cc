#include "gtest/gtest.h"

#include "heap.h"
#include "vector.h"

TEST(Heap_test, test_1) {
    auto test_sort = [](Array<int> expected, Array<int> array) {
        heap_sort(array.begin(), array.end());
        ASSERT_EQ(expected, array);
    };

    test_sort({1, 2, 3, 5, 9, 11}, {11, 5, 1, 3, 9, 2});
    test_sort({1, 2, 3}, {1, 3, 2});

    {
        Heap<int> heap;
        for (auto i : {11, 5, 1, 3, 9, 2})
            heap.push(i);

        Array<int> array(heap.size());
        for (auto el = array.end() - 1; el != array.begin() - 1; --el)
            *el = heap.pop();

        Array<int> expected{1, 2, 3, 5, 9, 11};
        ASSERT_EQ(expected, array);
        ASSERT_TRUE(heap.empty());
    }
    {
        Heap<int, std::greater<int>> heap;
        for (auto i : {11, 5, 1, 3, 9, 2})
            heap.push(i);
        
        Array<int> array(heap.size());
        for (auto& e : array)
            e = heap.pop();

        Array<int> expected{1, 2, 3, 5, 9, 11};
        ASSERT_EQ(expected, array);
        ASSERT_TRUE(heap.empty());
    }
    {
        Array<int> indices{0, 1, 2, 3, 4, 5};
        Array<int> weights{45, 21, 83, 1, 90, 2};
        struct Comparator {
            const Array<int>& weights_;
            Comparator(const Array<int>& weights) :weights_(weights) {}
            bool operator()(int i1, int i2) { return weights_[i1] > weights_[i2]; }
        };
        Heap<int, Comparator> heap(10, Comparator(weights));

        for (int i : Array<int>{4, 2, 5, 1, 3, 0})
            heap.push(i);

        Array<int> array(heap.size());
        for (auto& e : array)
            e = heap.pop();
        ASSERT_EQ(Array<int>({3, 5, 1, 0, 2, 4}), array);
    }
}
