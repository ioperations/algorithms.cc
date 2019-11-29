#pragma once

#include <stdexcept>

#include "array.h"
#include "binary_tree.h"

class Incomplete_tree_exception : public std::invalid_argument{
    public:
        Incomplete_tree_exception() :std::invalid_argument("incomplete tree") {}
};

template<typename T>
Array<T> tree_to_heap(Binary_tree_node<T>& root) {
    Forward_list<Binary_tree_node<T>*> queue;
    queue.push_back(&root);
    size_t size = 0;
    bool incomplete_occurred = false;
    while (!queue.empty()) {
        auto node = queue.pop_front();
        ++size;
        if (node->l_)
            queue.push_back(node->l_);
        else if (node->r_)
            throw Incomplete_tree_exception();

        if (node->r_) {
            queue.push_back(node->r_);
        } else if (node->l_ && incomplete_occurred)
            throw Incomplete_tree_exception();

        if (!node->l_ || !node->r_)
            incomplete_occurred = true;
    }
    Array<T> heap(size);
    size_t index = -1;
    queue.push_back(&root);
    while (!queue.empty()) {
        auto node = queue.pop_front();
        heap[++index] = node->value_;
        if (node->l_)
            queue.push_back(node->l_);
        if (node->r_)
            queue.push_back(node->r_);
    }
    return heap;
}

template<typename T>
Binary_tree_node<T> heap_to_tree(const Array<T>& heap) {
    Binary_tree_node<T> root(heap[0]);
    Array<Binary_tree_node<T>*> nodes(heap.size());
    nodes[0] = &root;
    for (size_t i = 1; i < heap.size(); ++i)
        nodes[i] = new Binary_tree_node<T>(heap[i]);
    auto get_node = [&nodes](size_t index) {
        return index < nodes.size()
            ? nodes[index]
            : nullptr;
    };
    for (size_t i = 0; i < heap.size(); ++i) {
        nodes[i]->l_ = get_node((i + 1) * 2 - 1);
        nodes[i]->r_ = get_node((i + 1) * 2);
    }
    return root;
}

template<typename T>
void print_heap(const Array<T>& heap) {
    std::cout << heap_to_tree(heap) << std::endl;
}
