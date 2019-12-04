#pragma once

#include <stdexcept>

#include "array.h"
#include "binary_tree.h"

class Incomplete_tree_exception : public std::invalid_argument{
    public:
        Incomplete_tree_exception() :std::invalid_argument("incomplete tree") {}
};

template<typename A>
void heap_fix_up(A& array_, size_t i) {
    ++i;
    for (; i > 1 && array_[i / 2 - 1] < array_[i - 1]; i /= 2)
        std::swap(array_[i - 1], array_[i / 2 - 1]);
}

template<typename A>
void heap_fix_down(A& a, size_t i, size_t size_) {
    ++i;
    while (i * 2 <= size_) {
        size_t j = 2 * i;
        if (j < size_ && a[j - 1] < a[j]) ++j;
        if (a[i - 1] < a[j - 1])
            std::swap(a[i - 1], a[j - 1]);
        i = j;
    }
}

template<typename It>
void heap_sort(const It& begin, const It& end) {
    for (size_t i = (end - begin) / 2 + 1; i > 0;) {
        heap_fix_down(begin, i - 1, end - begin);
        --i;
    }
    for (int n = end - begin - 1; n > 0; --n) {
        std::swap(begin[0], begin[n]);
        heap_fix_down(begin, 0, n);
    }
}

template<typename T>
class Heap {
    private:

        size_t array_size_;
        size_t size_;
        T* array_;
    public:
        Heap(size_t size) :array_size_(size), size_(0), array_(new T[array_size_]) {}
        Heap() :Heap(100) {}
        ~Heap() {
            delete[] array_;
        }

        Heap(const Heap&) = delete;
        Heap& operator=(const Heap&) = delete;

        Heap(Heap&& o) :array_size_(o.array_size_), size_(o.size_), array_(o.array_) {
            o.array_ = nullptr;
        }
        Heap& operator=(Heap&& o) {
            std::swap(array_size_, o.array_size_);
            std::swap(size_, o.size_);
            std::swap(array_, o.array_);
            return *this;
        }

        Heap(const Binary_tree_node<T>& root);
        Binary_tree_node<T> to_tree() const;

        template<typename TT>
            void push(TT&& t) {
                if (size_ > array_size_) {
                    array_size_ *= 2;
                    T* new_array = new T[array_size_];
                    for (size_t i = 0; i < size_; ++i)
                        new_array[i] = std::move(array_[i]);
                    delete[] array_;
                    array_ = new_array;
                }
                array_[size_] = std::forward<TT>(t);
                heap_fix_up(array_, size_);
                ++size_;
            }
        T pop() {
            --size_;
            std::swap(array_[0], array_[size_]);
            heap_fix_down(array_, 0, size_);
            return std::move(array_[size_]);
        }
        inline bool empty() const {
            return size_ == 0;
        }
        inline size_t size() const {
            return size_;
        }
};

template<typename T>
Heap<T>::Heap(const Binary_tree_node<T>& root) :array_size_(0), size_(0) {
    Forward_list<const Binary_tree_node<T>*> queue;
    queue.push_back(&root);
    bool incomplete_occurred = false;
    while (!queue.empty()) {
        auto node = queue.pop_front();
        ++size_;
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
    array_size_ = size_;
    array_ = new T[array_size_];
    queue.push_back(&root);
    for (size_t index = 0; !queue.empty(); ++index) {
        auto node = queue.pop_front();
        array_[index] = node->value_;
        heap_fix_up(array_, index);
        if (node->l_)
            queue.push_back(node->l_);
        if (node->r_)
            queue.push_back(node->r_);
    }
}

template<typename T>
Binary_tree_node<T> Heap<T>::to_tree() const {
    Binary_tree_node<T> root(array_[0]);
    Array<Binary_tree_node<T>*> nodes(size_);
    nodes[0] = &root;
    for (size_t i = 1; i < size_; ++i)
        nodes[i] = new Binary_tree_node<T>(array_[i]);
    auto get_node = [&nodes](size_t index) {
        return index < nodes.size()
            ? nodes[index]
            : nullptr;
    };
    for (size_t i = 0; i < size_; ++i) {
        nodes[i]->l_ = get_node((i + 1) * 2 - 1);
        nodes[i]->r_ = get_node((i + 1) * 2);
    }
    return root;
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, const Heap<T>& heap) {
    return stream << heap.to_tree() << std::endl;
}

