#pragma once

#include <stdexcept>

#include "array.h"
#include "binary_tree.h"

class Incomplete_tree_exception : public std::invalid_argument{
    public:
        Incomplete_tree_exception() :std::invalid_argument("incomplete tree") {}
};

template<typename A, typename C>
void heap_fix_up(A& array, size_t i, C c) {
    ++i;
    for (; i > 1 && c(array[i / 2 - 1], array[i - 1]); i /= 2)
        std::swap(array[i - 1], array[i / 2 - 1]);
}

template<typename A, typename C>
void heap_fix_down(A& a, size_t i, size_t size_, C c) {
    ++i;
    while (i * 2 <= size_) {
        size_t j = 2 * i;
        if (j < size_ && c(a[j - 1], a[j])) ++j;
        if (c(a[i - 1], a[j - 1]))
            std::swap(a[i - 1], a[j - 1]);
        i = j;
    }
}

template<typename It>
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

template<typename T, typename D>
class Heap_base {
    protected:
        size_t array_size_;
        size_t size_;
        T* array_;
        D* const d_;
    public:
        Heap_base(size_t size) :array_size_(size), size_(0), array_(new T[array_size_]), d_(static_cast<D*>(this)) {}
        ~Heap_base() { delete[] array_; }

        Heap_base(const Heap_base&) = delete;
        Heap_base& operator=(const Heap_base&) = delete;

        Heap_base(Heap_base&& o) :array_size_(o.array_size_), size_(o.size_), array_(o.array_) {
            o.array_ = nullptr;
        }
        Heap_base& operator=(Heap_base&& o) {
            std::swap(array_size_, o.array_size_);
            std::swap(size_, o.size_);
            std::swap(array_, o.array_);
            return *this;
        }

        Heap_base(const Binary_tree_node<T>& root);
        Binary_tree_node<T> to_tree() const;

        void fix_up(size_t i) {
            ++i;
            for (; i > 1 && d_->compare(array_[i / 2 - 1], array_[i - 1]); i /= 2)
                do_swap(i - 1, i / 2 - 1);
        }
        void fix_down(size_t i) {
            ++i;
            while (i * 2 <= size_) {
                size_t j = 2 * i;
                if (j < size_ && d_->compare(array_[j - 1], array_[j])) ++j;
                if (d_->compare(array_[i - 1], array_[j - 1]))
                    do_swap(i - 1, j - 1);
                i = j;
            }
        }

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
                d_->set_value(size_, std::forward<TT>(t));
                fix_up(size_);
                ++size_;
            }
        void do_swap(size_t i, size_t j) {
            T t = std::move(array_[i]);
            d_->set_value(i, std::move(array_[j]));
            d_->set_value(j, std::move(t));
        }
        template<typename TT>
            void set_value(size_t i, TT&& t) {
                array_[i] = std::forward<TT>(t);
            }
        T pop() {
            --size_;
            do_swap(0, size_);
            fix_down(0);
            return std::move(array_[size_]);
        }
        inline bool empty() const {
            return size_ == 0;
        }
        inline size_t size() const {
            return size_;
        }
};

template<typename T, typename D>
Heap_base<T, D>::Heap_base(const Binary_tree_node<T>& root) :array_size_(0), size_(0) {
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
        fix_up(index);
        if (node->l_)
            queue.push_back(node->l_);
        if (node->r_)
            queue.push_back(node->r_);
    }
}

template<typename T, typename D>
Binary_tree_node<T> Heap_base<T, D>::to_tree() const {
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

template<typename T, typename D>
std::ostream& operator<<(std::ostream& stream, const Heap_base<T, D>& heap) {
    return stream << heap.to_tree() << std::endl;
}

template<typename T, typename C = std::less<T>>
class Heap : public Heap_base<T, Heap<T, C>> {
    private:
        using Base = Heap_base<T, Heap<T, C>>;
        C comparator_;
    public:
        Heap(size_t size, C c = {}) :Base(size), comparator_(c) {}
        bool compare(const T& t1, const T& t2) {
            return comparator_(t1, t2);
        }
};

struct Default_index_convertor {
    size_t operator()(size_t index) { return index; }
};

template<typename T, typename D>
class Multiway_heap_base : public Heap_base<T, D> {
    private:
        using Base = Heap_base<T, D>;
        size_t* inverted_;
    public:
        Multiway_heap_base(size_t size) :Base(size), inverted_(new size_t[size]) {}
        ~Multiway_heap_base() { delete inverted_; }
        template<typename TT>
            void set_value(size_t i, TT&& t) {
                Base::array_[i] = std::forward<TT>(t);
                inverted_[Base::d_->get_index(Base::array_[i])] = i;
            }
        void move_up(const T& value) {
            Base::fix_up(inverted_[Base::d_->get_index(value)]);
        }
        void move_down(const T& value) {
            Base::fix_down(inverted_[Base::d_->get_index(value)]);
        }
        size_t get_index(T value) {
            return Base::d_->get_index(value);
        }
};

template<typename T, typename C = std::less<T>, typename CR = Default_index_convertor>
class Multiway_heap : public Multiway_heap_base<T, Multiway_heap<T, C, CR>> {
    private:
        using Base = Multiway_heap_base<T, Multiway_heap<T, C, CR>>;
        C comparator_;
        CR index_convertor_;
    public:
        Multiway_heap(size_t size, C c = {}, CR index_convertor = {}) 
            :Base(size), comparator_(c), index_convertor_(index_convertor)
            {}
        bool compare(const T& t1, const T& t2) {
            return comparator_(t1, t2);
        }
        size_t get_index(const T& value) {
            return index_convertor_(value);
        }
};

