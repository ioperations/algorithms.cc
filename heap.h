#pragma once

#include <stdexcept>

#include "array.h"
#include "binary_tree.h"

class IncompleteTreeException : public std::invalid_argument {
   public:
    IncompleteTreeException() : std::invalid_argument("incomplete tree") {}
};

template <typename A, typename C>
void heap_fix_up(A& array, size_t i, C c) {
    ++i;
    for (; i > 1 && c(array[i / 2 - 1], array[i - 1]); i /= 2)
        std::swap(array[i - 1], array[i / 2 - 1]);
}

template <typename A, typename C>
void heap_fix_down(A& a, size_t i, size_t size, C c) {
    ++i;
    while (i * 2 <= size) {
        size_t j = 2 * i;
        if (j < size && c(a[j - 1], a[j])) ++j;
        if (c(a[i - 1], a[j - 1])) std::swap(a[i - 1], a[j - 1]);
        i = j;
    }
}

template <typename It>
void heap_sort(const It& begin, const It& end) {
    std::less<typename std::iterator_traits<It>::value_type> c;
    for (size_t i = (end - begin) / 2 + 1; i > 0;) {
        heap_fix_down(begin, i - 1, end - begin, c);
        --i;
    }
    for (int n = end - begin - 1; n > 0; --n) {
        std::swap(begin[0], begin[n]);
        heap_fix_down(begin, 0, n, c);
    }
}

template <typename T, typename D>
class HeapBase {
   protected:
    size_t m_array_size;
    size_t m_size;
    T* m_array;
    D* const m_d;

   public:
    HeapBase(size_t size)
        : m_array_size(size),
          m_size(0),
          m_array(new T[m_array_size]),
          m_d(static_cast<D*>(this)) {}
    ~HeapBase() { delete[] m_array; }

    HeapBase(const HeapBase&) = delete;
    HeapBase& operator=(const HeapBase&) = delete;

    HeapBase(HeapBase&& o)
        : m_array_size(o.m_array_size), m_size(o.m_size), m_array(o.m_array) {
        o.m_array = nullptr;
    }
    HeapBase& operator=(HeapBase&& o) {
        std::swap(m_array_size, o.m_array_size);
        std::swap(m_size, o.m_size);
        std::swap(m_array, o.m_array);
        return *this;
    }

    HeapBase(const BinaryTreeNode<T>& root);
    BinaryTreeNode<T> to_tree() const;

    void fix_up(size_t i) {
        ++i;
        for (; i > 1 && m_d->compare(m_array[i / 2 - 1], m_array[i - 1]);
             i /= 2)
            do_swap(i - 1, i / 2 - 1);
    }
    void fix_down(size_t i) {
        ++i;
        while (i * 2 <= m_size) {
            size_t j = 2 * i;
            if (j < m_size && m_d->compare(m_array[j - 1], m_array[j])) ++j;
            if (m_d->compare(m_array[i - 1], m_array[j - 1]))
                do_swap(i - 1, j - 1);
            i = j;
        }
    }

    template <typename TT>
    void push(TT&& t) {
        if (m_size > m_array_size) {
            m_array_size *= 2;
            T* new_array = new T[m_array_size];
            for (size_t i = 0; i < m_size; ++i)
                new_array[i] = std::move(m_array[i]);
            delete[] m_array;
            m_array = new_array;
        }
        m_d->set_value(m_size, std::forward<TT>(t));
        fix_up(m_size);
        ++m_size;
    }
    void do_swap(size_t i, size_t j) {
        T t = std::move(m_array[i]);
        m_d->set_value(i, std::move(m_array[j]));
        m_d->set_value(j, std::move(t));
    }
    template <typename TT>
    void set_value(size_t i, TT&& t) {
        m_array[i] = std::forward<TT>(t);
    }
    T pop() {
        --m_size;
        do_swap(0, m_size);
        fix_down(0);
        return std::move(m_array[m_size]);
    }
    inline bool empty() const { return m_size == 0; }
    inline size_t size() const { return m_size; }
};

template <typename T, typename D>
HeapBase<T, D>::HeapBase(const BinaryTreeNode<T>& root)
    : m_array_size(0), m_size(0) {
    ForwardList<const BinaryTreeNode<T>*> queue;
    queue.push_back(&root);
    bool incomplete_occurred = false;
    while (!queue.empty()) {
        auto node = queue.pop_front();
        ++m_size;
        if (node->l_)
            queue.push_back(node->l_);
        else if (node->r_)
            throw IncompleteTreeException();

        if (node->r_) {
            queue.push_back(node->r_);
        } else if (node->l_ && incomplete_occurred)
            throw IncompleteTreeException();

        if (!node->l_ || !node->r_) incomplete_occurred = true;
    }
    m_array_size = m_size;
    m_array = new T[m_array_size];
    queue.push_back(&root);
    for (size_t index = 0; !queue.empty(); ++index) {
        auto node = queue.pop_front();
        m_array[index] = node->value_;
        fix_up(index);
        if (node->l_) queue.push_back(node->l_);
        if (node->r_) queue.push_back(node->r_);
    }
}

template <typename T, typename D>
BinaryTreeNode<T> HeapBase<T, D>::to_tree() const {
    BinaryTreeNode<T> root(m_array[0]);
    Array<BinaryTreeNode<T>*> nodes(m_size);
    nodes[0] = &root;
    for (size_t i = 1; i < m_size; ++i)
        nodes[i] = new BinaryTreeNode<T>(m_array[i]);
    auto get_node = [&nodes](size_t index) {
        return index < nodes.size() ? nodes[index] : nullptr;
    };
    for (size_t i = 0; i < m_size; ++i) {
        nodes[i]->l_ = get_node((i + 1) * 2 - 1);
        nodes[i]->r_ = get_node((i + 1) * 2);
    }
    return root;
}

template <typename T, typename D>
std::ostream& operator<<(std::ostream& stream, const HeapBase<T, D>& heap) {
    return stream << heap.to_tree() << std::endl;
}

template <typename T, typename C = std::less<T>>
class Heap : public HeapBase<T, Heap<T, C>> {
   private:
    using Base = HeapBase<T, Heap<T, C>>;
    C m_comparator;

   public:
    Heap(size_t size, C c = {}) : Base(size), m_comparator(c) {}
    bool compare(const T& t1, const T& t2) { return m_comparator(t1, t2); }
};

struct DefaultIndexConvertor {
    size_t operator()(size_t index) { return index; }
};

template <typename T, typename D>
class MultiwayHeapBase : public HeapBase<T, D> {
   private:
    using Base = HeapBase<T, D>;
    size_t* inverted;

   public:
    MultiwayHeapBase(size_t size) : Base(size), inverted(new size_t[size]) {}
    ~MultiwayHeapBase() { delete[] inverted; }
    template <typename TT>
    void set_value(size_t i, TT&& t) {
        Base::m_array[i] = std::forward<TT>(t);
        inverted[Base::m_d->get_index(Base::m_array[i])] = i;
    }
    void move_up(const T& value) {
        Base::fix_up(inverted[Base::m_d->get_index(value)]);
    }
    void move_down(const T& value) {
        Base::fix_down(inverted[Base::m_d->get_index(value)]);
    }
    size_t get_index(T value) { return Base::m_d->get_index(value); }
};

template <typename T, typename C = std::less<T>,
          typename CR = DefaultIndexConvertor>
class MultiwayHeap : public MultiwayHeapBase<T, MultiwayHeap<T, C, CR>> {
   private:
    using Base = MultiwayHeapBase<T, MultiwayHeap<T, C, CR>>;
    C m_comparator;
    CR m_index_convertor;

   public:
    MultiwayHeap(size_t size, C c = {}, CR index_convertor = {})
        : Base(size), m_comparator(c), m_index_convertor(index_convertor) {}
    bool compare(const T& t1, const T& t2) { return m_comparator(t1, t2); }
    size_t get_index(const T& value) { return m_index_convertor(value); }
};
