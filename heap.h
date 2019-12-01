#pragma once

#include <stdexcept>

#include "array.h"
#include "binary_tree.h"

class Incomplete_tree_exception : public std::invalid_argument{
    public:
        Incomplete_tree_exception() :std::invalid_argument("incomplete tree") {}
};

template<typename T>
class Heap {
    private:
        Array<T> array_;
        template<typename TT>
            static Array<TT> from_tree(const Binary_tree_node<TT>& root);
        template<typename TT>
            static inline void fix_up(Array<TT>& array, size_t i);
    public:
        Heap(const Binary_tree_node<T>& root) :array_(Heap<T>::from_tree(root)) {}
        Binary_tree_node<T> to_tree() const;

        inline void fix_up(size_t i) { // todo remove?
            Heap<T>::fix_up(array_, i);
        }
        void fix_down(size_t i) {
            ++i;
            auto& a = array_;
            while (i * 2 <= a.size()) {
                size_t j = 2 * i;
                if (j <= array_.size() && a[j - 1] < a[j]) ++j;
                if (a[i - 1] < a[j - 1]) {
                    std::swap(a[i - 1], a[j - 1]);
                    i = j;
                }
            }
        }
        inline T& operator[](size_t i) {
            return array_[i];
        }
};

template<typename T>
template<typename TT>
void Heap<T>::fix_up(Array<TT>& array, size_t i) {
    ++i;
    for (; i > 1 && array[i / 2 - 1] < array[i - 1]; i /= 2)
        std::swap(array[i - 1], array[i / 2 - 1]);
}

template<typename T>
template<typename TT>
Array<TT> Heap<T>::from_tree(const Binary_tree_node<TT>& root) {
    Forward_list<const Binary_tree_node<TT>*> queue;
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
    Array<TT> heap(size);
    queue.push_back(&root);
    for (size_t index = 0; !queue.empty(); ++index) {
        auto node = queue.pop_front();
        heap[index] = node->value_;
        // fix_up(heap, index);
        if (node->l_)
            queue.push_back(node->l_);
        if (node->r_)
            queue.push_back(node->r_);
    }
    return heap;
}

template<typename T>
Binary_tree_node<T> Heap<T>::to_tree() const {
    Binary_tree_node<T> root(array_[0]);
    Array<Binary_tree_node<T>*> nodes(array_.size());
    nodes[0] = &root;
    for (size_t i = 1; i < array_.size(); ++i)
        nodes[i] = new Binary_tree_node<T>(array_[i]);
    auto get_node = [&nodes](size_t index) {
        return index < nodes.size()
            ? nodes[index]
            : nullptr;
    };
    for (size_t i = 0; i < array_.size(); ++i) {
        nodes[i]->l_ = get_node((i + 1) * 2 - 1);
        nodes[i]->r_ = get_node((i + 1) * 2);
    }
    return root;
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, const Heap<T>& heap) {
    return stream << heap.to_tree() << std::endl;
}
