#pragma once

#include "array.h"
#include "heap.h"

namespace Graph {

enum class Graph_type { GRAPH, DIGRAPH };

template <typename T>
class Vertex_base {
   protected:
    T value_;
    Vertex_base(const T& value) : value_(value) {}
    Vertex_base() = default;

   public:
    using value_type = T;
    const T& value() const { return value_; }
    void set_value(const T& value) { value_ = value; }
    bool operator==(const Vertex_base& o) const { return this == &o; }
    bool operator!=(const Vertex_base& o) const { return !operator==(o); }
    friend std::ostream& operator<<(std::ostream& stream,
                                    const Vertex_base& v) {
        return stream << v.value_;
    }
};

template <typename V, typename E, bool T_is_const,
          typename ET = typename E::value_type>
class Edges_iterator_entry {
   public:
    using vertex_type = std::conditional_t<T_is_const, const V, V>;
    using edge_type = std::conditional_t<T_is_const, const E, E>;

    vertex_type* source_;
    vertex_type* target_;  // todo delete
    edge_type* edge_;

    Edges_iterator_entry() = default;
    Edges_iterator_entry(vertex_type* source) : source_(source) {}

    vertex_type& source() const { return *source_; }
    vertex_type& target() const { return *target_; }
    edge_type& edge() const { return *edge_; }
};

struct Array_cycle {
   private:
    Array<size_t> array_;
    size_t begin_index_;

    struct Iterator {
       private:
        const Array<size_t>& cycle_;
        size_t index_;

       public:
        Iterator(const Array<size_t>& cycle, size_t index)
            : cycle_(cycle), index_(index) {}
        bool operator!=(const Iterator& o) const { return index_ != o.index_; }
        Iterator& operator++() {
            index_ = cycle_[index_];
            return *this;
        }
        size_t operator*() { return index_; }
    };

   public:
    Array_cycle(Array<size_t>&& array, size_t sentinel)
        : array_(std::move(array)) {
        for (size_t i = 0; i < array_.size(); ++i)
            if (array_[i] != sentinel) {
                begin_index_ = i;
                break;
            }
    }
    Iterator cbegin() const { return Iterator(array_, begin_index_); }
    bool empty() const { return array_.size() == 0; }
};

template <typename V, typename W>
class Vertex_heap : public Multiway_heap_base<V, Vertex_heap<V, W>> {
   private:
    using Base = Multiway_heap_base<V, Vertex_heap<V, W>>;
    Array<W>& weights_;

   public:
    Vertex_heap(size_t size, Array<W>& weights)
        : Base(size), weights_(weights) {}
    bool compare(const V& v1, const V& v2) {
        return weights_[*v1] > weights_[*v2];
    }
    size_t get_index(const V& v) { return *v; }
};

}  // namespace Graph
