#pragma once

#include "array.h"
#include "heap.h"

namespace Graph {

enum class GraphType { GRAPH, DIGRAPH };

template <typename T>
class VertexBase {
   protected:
    T m_value;
    VertexBase(const T& value) : m_value(value) {}
    VertexBase() = default;

   public:
    using value_type = T;
    const T& value() const { return m_value; }
    void set_value(const T& value) { m_value = value; }
    bool operator==(const VertexBase& o) const { return this == &o; }
    bool operator!=(const VertexBase& o) const { return !operator==(o); }
    friend std::ostream& operator<<(std::ostream& stream, const VertexBase& v) {
        return stream << v.m_value;
    }
};

template <typename V, typename E, bool T_is_const,
          typename ET = typename E::value_type>
class EdgesIteratorEntry {
   public:
    using vertex_type = std::conditional_t<T_is_const, const V, V>;
    using edge_type = std::conditional_t<T_is_const, const E, E>;

    vertex_type* m_source;
    vertex_type* m_target;  // todo delete
    edge_type* m_edge;

    EdgesIteratorEntry() = default;
    EdgesIteratorEntry(vertex_type* source) : m_source(source) {}

    vertex_type& source() const { return *m_source; }
    vertex_type& target() const { return *m_target; }
    edge_type& edge() const { return *m_edge; }
};

struct ArrayCycle {
   private:
    Array<size_t> m_array;
    size_t m_begin_index;

    struct Iterator {
       private:
        const Array<size_t>& m_cycle;
        size_t m_index;

       public:
        Iterator(const Array<size_t>& cycle, size_t index)
            : m_cycle(cycle), m_index(index) {}
        bool operator!=(const Iterator& o) const {
            return m_index != o.m_index;
        }
        Iterator& operator++() {
            m_index = m_cycle[m_index];
            return *this;
        }
        size_t operator*() { return m_index; }
    };

   public:
    ArrayCycle(Array<size_t>&& array, size_t sentinel)
        : m_array(std::move(array)) {
        for (size_t i = 0; i < m_array.size(); ++i)
            if (m_array[i] != sentinel) {
                m_begin_index = i;
                break;
            }
    }
    Iterator cbegin() const { return Iterator(m_array, m_begin_index); }
    bool empty() const { return m_array.size() == 0; }
};

template <typename V, typename W>
class VertexHeap : public MultiwayHeapBase<V, VertexHeap<V, W>> {
   private:
    using Base = MultiwayHeapBase<V, VertexHeap<V, W>>;
    Array<W>& m_weights;

   public:
    VertexHeap(size_t size, Array<W>& weights)
        : Base(size), m_weights(weights) {}
    bool compare(const V& v1, const V& v2) {
        return m_weights[*v1] > m_weights[*v2];
    }
    size_t get_index(const V& v) { return *v; }
};

}  // namespace Graph
