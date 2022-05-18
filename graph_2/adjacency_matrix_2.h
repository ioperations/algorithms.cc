#pragma once

#include <vector>

#include "array.h"
#include "graph_2.h"
#include "two_dimensional_array.h"

namespace Graph_2 {

namespace Adjency_matrix_ns {

class Vertex {
   private:
    size_t m_index;

   public:
    Vertex(size_t index) : m_index(index) {}
    Vertex() : Vertex(0) {}
    operator size_t() { return m_index; }
};

class AdjacentIteratorEnd {
   private:
    template <typename T>
    friend class Adjency_matrix;
    static const AdjacentIteratorEnd instance() {
        static const AdjacentIteratorEnd e;
        return e;
    }
};

class AdjacentIterator {
   private:
    template <typename T>
    friend class Adjency_matrix;
    bool* m_begin;
    bool* m_end;
    bool* m_current;
    AdjacentIterator(bool* begin, bool* end, bool* current)
        : m_begin(begin), m_end(end), m_current(current) {}
    AdjacentIterator& move_on_first() {
        while (!*m_current && m_current != m_end) ++m_current;
        return *this;
    }

   public:
    AdjacentIterator() = default;
    AdjacentIterator& operator++() {
        ++m_current;
        return move_on_first();
    }
    bool operator!=(const AdjacentIterator& o) {
        return m_current != o.m_current;
    }
    bool operator!=(const AdjacentIteratorEnd& e) { return m_current != m_end; }
    Vertex operator*() { return m_current - m_begin; }
};

template <typename T>
class Adjency_matrix {
   private:
    Array<T> m_vertices;
    Two_dimensional_array<bool> m_edges;
    size_t m_vertices_count;

   public:
    using value_type = T;
    using vertex_type = Vertex;
    using adjacent_iterator = AdjacentIterator;
    using adjacent_iterator_end = AdjacentIteratorEnd;

    Adjency_matrix(size_t size = 100)
        : m_vertices(size), m_edges(size, size), m_vertices_count(0) {
        m_edges.fill(false);
    }
    template <typename TT>
    vertex_type add_vertex(TT&& t) {
        auto index = m_vertices_count;
        m_vertices[index] = std::forward<TT>(t);
        ++m_vertices_count;
        return index;
    }
    T& value(vertex_type v) { return m_vertices[v]; }
    void add_edge(vertex_type v, vertex_type w) {
        m_edges.get(v, w) = true;
        m_edges.get(w, v) = true;
    }
    Pair<adjacent_iterator, adjacent_iterator_end> adjacent(vertex_type v) {
        auto r = m_edges[v];
        bool* b = r.begin();
        bool* e = r.end();
        return {AdjacentIterator(b, e, b).move_on_first(),
                AdjacentIteratorEnd::instance()};
    }

    void print_internal(std::ostream& stream) {
        for (size_t r = 0; r < m_vertices_count; ++r) {
            for (size_t c = 0; c < m_vertices_count; ++c)
                stream << m_edges.get(r, c) << " ";
            stream << std::endl;
        }
    }
};
}  // namespace Adjency_matrix_ns

template <typename T>
using Adjency_matrix = typename Adjency_matrix_ns::Adjency_matrix<T>;

}  // namespace Graph_2
