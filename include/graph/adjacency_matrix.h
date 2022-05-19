#pragma once

#include <iostream>

#include "graph_common.h"
#include "vector.h"

namespace Graph {

namespace Adjacency_matrix_ns {

template <typename T>
class Edge {
   private:
    T m_weight;

   public:
    static constexpr T empty_value() { return -1; }
    using value_type = T;
    Edge() : m_weight(empty_value()) {}
    Edge(const T& t) : m_weight(t) {}
    T weight() const { return m_weight; }
    void set_weight(const T& weight) { m_weight = weight; }
    bool exists() const { return m_weight != empty_value(); }
    operator bool() const { return exists(); }
};

template <>
constexpr bool Edge<bool>::empty_value() {
    return false;
}

template <typename V, typename E>
class AdjacencyMatrixBase {
   private:
    class Vertex;
    friend class Vertex;
    Vector<Vertex> m_vertices;
    void update_vertices_this_link() {
        for (auto& v : m_vertices) v.m_matrix = this;
    }

   protected:
    using vertex_type = Vertex;
    Vector<Vector<E>> m_edges;

   public:
    using value_type = V;
    using edge_type = E;

    AdjacencyMatrixBase() : m_edges(100) {
        for (auto& l : m_edges) l = Vector<E>(100);
    }

    AdjacencyMatrixBase(const AdjacencyMatrixBase& o)
        : m_vertices(o.m_vertices), m_edges(o.m_edges) {
        update_vertices_this_link();
    }
    AdjacencyMatrixBase& operator=(const AdjacencyMatrixBase& o) {
        auto copy = o;
        std::swap(*this, copy);
        return *this;
    }
    AdjacencyMatrixBase(AdjacencyMatrixBase&& o)
        : m_vertices(std::move(o.m_vertices)), m_edges(std::move(o.m_edges)) {
        update_vertices_this_link();
    }
    AdjacencyMatrixBase& operator=(AdjacencyMatrixBase&& o) {
        std::swap(m_vertices, o.m_vertices);
        std::swap(m_edges, o.m_edges);
        update_vertices_this_link();
        return *this;
    }

    vertex_type& create_vertex(const V& t) {
        m_vertices.emplace_back(t, this);
        return m_vertices[m_vertices.size() - 1];
    }
    size_t vertices_count() const { return m_vertices.size(); }

    const vertex_type& operator[](size_t index) const {
        return m_vertices[index];
    }
    vertex_type& operator[](size_t index) { return m_vertices[index]; }
    bool has_edge(const vertex_type& v, const vertex_type& w) {
        return m_edges[v][w];
    }
    const E* get_edge(const vertex_type& v, const vertex_type& w) const {
        auto& edge = m_edges[v][w];
        return edge.exists() ? &edge : nullptr;
    }
    E* get_edge(const vertex_type& v, const vertex_type& w) {
        return const_cast<E*>(
            static_cast<const AdjacencyMatrixBase*>(this)->get_edge(v, w));
    }

    auto begin() { return m_vertices.begin(); }
    auto end() { return m_vertices.end(); }
    auto cbegin() const { return m_vertices.cbegin(); }
    auto cend() const { return m_vertices.cend(); }
    auto crbegin() const { return m_vertices.crbegin(); }
    auto crend() const { return m_vertices.crend(); }

    void print_internal(std::ostream& stream) const {
        auto size = m_vertices.size();
        for (size_t r = 0; r < size; ++r) {
            for (size_t c = 0; c < size; ++c) {
                stream << m_edges[r][c] << " ";
            }
            stream << std::endl;
        }
    }
};

template <typename D, GraphType graph_type, typename V, typename E,
          typename ET = typename E::value_type>
class EdgesHandler : public AdjacencyMatrixBase<V, E> {
   public:
    using vertex_type = typename AdjacencyMatrixBase<V, E>::vertex_type;
    D& add_edge(const vertex_type& v1, const vertex_type& v2, const E& e) {
        auto& d = *static_cast<D*>(this);
        d.set_edge(v1, v2, e);
        return d;
    }
    void remove_edge(const vertex_type& v1, const vertex_type& v2) {
        static_cast<D*>(this)->set_edge(v1, v2, E::empty_value());
    }
};

template <typename D, GraphType graph_type, typename V, typename E>
class EdgesHandler<D, graph_type, V, E, bool>
    : public AdjacencyMatrixBase<V, E> {
   public:
    using vertex_type = typename AdjacencyMatrixBase<V, E>::vertex_type;
    D& add_edge(const vertex_type& v1, const vertex_type& v2) {
        auto& d = *static_cast<D*>(this);
        d.set_edge(v1, v2, true);
        return d;
    }
    void remove_edge(const vertex_type& v1, const vertex_type& v2) {
        static_cast<D*>(this)->set_edge(v1, v2, false);
    }
};

template <GraphType graph_type, typename V, typename E,
          typename ET = typename E::value_type>
class AdjacencyMatrix
    : public EdgesHandler<AdjacencyMatrix<graph_type, V, E, ET>, graph_type, V,
                          E, ET> {
   public:
    using Base = EdgesHandler<AdjacencyMatrix<graph_type, V, E, ET>, graph_type,
                              V, E, ET>;
    using vertex_type = typename Base::vertex_type;
    void set_edge(const vertex_type& v1, const vertex_type& v2, const E& e) {
        Base::m_edges[v1][v2] = e;
        Base::m_edges[v2][v1] = e;
    }
};

template <typename V, typename E, typename ET>
class AdjacencyMatrix<GraphType::DIGRAPH, V, E, ET>
    : public EdgesHandler<AdjacencyMatrix<GraphType::DIGRAPH, V, E, ET>,
                          GraphType::DIGRAPH, V, E, ET> {
   public:
    using Base = EdgesHandler<AdjacencyMatrix<GraphType::DIGRAPH, V, E, ET>,
                              GraphType::DIGRAPH, V, E, ET>;
    using vertex_type = typename Base::vertex_type;
    void set_edge(const vertex_type& v1, const vertex_type& v2, const E& e) {
        Base::m_edges[v1][v2] = e;
    }
};

template <typename V, typename E>
class AdjacencyMatrixBase<V, E>::Vertex : public VertexBase<V> {
   private:
    template <bool T_is_const>
    class Iterator;
    template <bool T_is_const>
    class Edges_iterator;
    friend AdjacencyMatrixBase;  // Adjacency_matrix_base;
    friend class Vector<Vertex>;

    AdjacencyMatrixBase* m_matrix;

    Vertex(const V& value, AdjacencyMatrixBase* matrix)
        : VertexBase<V>(value), m_matrix(matrix) {}
    Vertex() : m_matrix(nullptr) {}

    Vector<E>& edges() const { return m_matrix->m_edges[*this]; }
    template <typename It, typename VV>
    static It create_begin_it(VV* vertex) {
        return It(vertex, vertex->edges().begin()).move_to_non_empty();
    }
    template <typename It, typename VV>
    static It create_end_it(VV* vertex) {
        return It(vertex, vertex->edges().end());
    }

   public:
    size_t index() const { return this - m_matrix->m_vertices.cbegin(); }
    operator size_t() const { return index(); }

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    using edges_iterator = Edges_iterator<false>;
    using const_edges_iterator = Edges_iterator<true>;

    auto cbegin() const { return create_begin_it<const_iterator>(this); }
    auto cend() const { return create_end_it<const_iterator>(this); }
    auto begin() { return create_begin_it<iterator>(this); }
    auto end() { return create_end_it<iterator>(this); }

    auto cedges_begin() const {
        return create_begin_it<const_edges_iterator>(this);
    }
    auto cedges_end() const {
        return create_end_it<const_edges_iterator>(this);
    }
    auto edges_begin() { return create_begin_it<edges_iterator>(this); }
    auto edges_end() { return create_end_it<edges_iterator>(this); }
};

template <typename V, typename E>
template <bool T_is_const>
class AdjacencyMatrixBase<V, E>::Vertex::Iterator {
   private:
    using value_type = std::conditional_t<T_is_const, const Vertex, Vertex>;
    using vertices_type =
        std::conditional_t<T_is_const, const Vector<Vertex>, Vector<Vertex>>;
    friend class Vertex;
    Iterator& move_to_non_empty() {
        for (; m_it != m_edges.end() && !*m_it; ++m_it)
            ;
        return *this;
    }
    Iterator(value_type* vertex, const typename Vector<E>::iterator& it)
        : m_vertices(vertex->m_matrix->m_vertices),
          m_edges(vertex->m_matrix->m_edges[vertex->index()]),
          m_it(it) {}
    static Iterator create_begin_it(value_type* vertex) {
        return Iterator(vertex, vertex->edges().begin()).move_to_non_empty();
    }
    static Iterator create_end_it(value_type* vertex) {
        return {vertex, vertex->edges().end()};
    }

   protected:
    vertices_type& m_vertices;
    Vector<E>& m_edges;
    typename Vector<E>::iterator m_it;

   public:
    Iterator& operator++() {
        ++m_it;
        move_to_non_empty();
        return *this;
    }
    bool operator==(const Iterator& o) const { return m_it == o.m_it; }
    bool operator!=(const Iterator& o) const { return !operator==(o); }
    value_type& operator*() const { return m_vertices[m_it - m_edges.begin()]; }
    value_type* operator->() const { return &operator*(); }
};

template <typename V, typename E>
template <bool T_is_const>
class AdjacencyMatrixBase<V, E>::Vertex::Edges_iterator
    : public Iterator<T_is_const> {
   public:
    using entry_type = EdgesIteratorEntry<Vertex, E, T_is_const>;

   private:
    using Base = Iterator<T_is_const>;
    using vertex_type = std::conditional_t<T_is_const, const Vertex, Vertex>;
    friend class Vertex;

    entry_type m_entry;

    Edges_iterator(vertex_type* vertex, const typename Vector<E>::iterator& it)
        : Base(vertex, it), m_entry(vertex) {}
    Edges_iterator& move_to_non_empty() {
        Base::move_to_non_empty();
        update_entry();
        return *this;
    }
    template <typename VV, typename EE, bool TT_is_const, typename It>
    static void update_edge_p(EdgesIteratorEntry<VV, EE, TT_is_const>& entry,
                              const It& it) {
        entry.m_edge = &*it;
    }
    template <typename VV, bool TT_is_const, typename It>
    static void update_edge_p(
        EdgesIteratorEntry<VV, Edge<bool>, TT_is_const>& entry, const It& it) {}
    void update_entry() {
        if (Base::m_it != Base::m_edges.end()) {
            m_entry.m_target =
                &Base::m_vertices[Base::m_it - Base::m_edges.begin()];
            update_edge_p(m_entry, Base::m_it);
        }
    }

   public:
    const entry_type& operator*() const { return m_entry; }
    const entry_type* operator->() const { return &m_entry; }
    Edges_iterator& operator++() {
        Base::operator++();
        update_entry();
        return *this;
    }
};

}  // namespace Adjacency_matrix_ns

template <GraphType graph_type, typename V, typename TE = bool,
          typename E = Adjacency_matrix_ns::Edge<TE>>
using AdjacencyMatrix = Adjacency_matrix_ns::AdjacencyMatrix<graph_type, V, E>;

template <GraphType graph_type, typename V, typename E>
void print_representation(
    const Adjacency_matrix_ns::AdjacencyMatrix<graph_type, V, E>& g,
    std::ostream& stream) {
    g.print_internal(stream);
}

}  // namespace Graph
