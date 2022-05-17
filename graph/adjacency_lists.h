#pragma once

#include <iostream>

#include "forward_list.h"
#include "graph_common.h"
#include "vector.h"

namespace Graph {

namespace Adjacency_lists_ns {

template <typename T>
class Edge {
   protected:
    T m_weight;

   public:
    using value_type = T;
    Edge(const T& t) : m_weight(t) {}
    T weight() const { return m_weight; }
    void set_weight(const T& weight) { m_weight = weight; }
};

template <typename T>
class FlowEdge : public Edge<T> {
   private:
    T m_flow;

   public:
    FlowEdge(const T& cap, const T& flow) : Edge<T>(cap), m_flow(flow) {}
    T cap() const { return Edge<T>::m_weight; }
    T flow() const { return m_flow; }
    void set_flow(const T& flow) { m_flow = flow; }
};

class VertexLinkBase {
   private:
    size_t m_target;

   public:
    VertexLinkBase(size_t target) : m_target(target) {}
    size_t target() const { return m_target; }
};

template <GraphType T_graph_type, typename E>
class VertexLink : public VertexLinkBase, public E {
   public:
    VertexLink(size_t target, const E& edge)
        : VertexLinkBase(target), E(edge) {}
};

template <GraphType T_graph_type>
class VertexLink<T_graph_type, Edge<bool>> : public VertexLinkBase {
   public:
    using value_type = bool;
    VertexLink(size_t target, const Edge<bool>& edge)
        : VertexLinkBase(target) {}
};

template <GraphType TT_graph_type, typename VV>
class AdjacencyListsBase;

template <GraphType T_graph_type, typename VT, typename E, typename D>
class AdjListsVertexBase : public VertexBase<VT> {
   private:
    template <bool T_is_const>
    class Iterator;
    template <bool T_is_const>
    class Edges_iterator;
    template <GraphType TT_graph_type, typename VV, typename ET>
    friend class AdjacencyLists;

    using vertex_link_type = VertexLink<T_graph_type, E>;

    inline D* derived() { return static_cast<D*>(this); }
    inline const D* derived() const { return static_cast<const D*>(this); }

   protected:
    using adj_lists_type = AdjacencyListsBase<T_graph_type, D>;
    adj_lists_type* m_adjacency_lists;
    Forward_list<vertex_link_type> m_links;

    AdjListsVertexBase(const VT& value, adj_lists_type* adjacency_lists)
        : VertexBase<VT>(value), m_adjacency_lists(adjacency_lists) {}
    AdjListsVertexBase() : m_adjacency_lists(nullptr) {}

    bool link_exists(const AdjListsVertexBase& v) {
        for (auto link = m_links.begin(); link != m_links.end(); ++link)
            if (link->target() == v.index()) return true;
        return false;
    }
    void add_link(const AdjListsVertexBase& v, const E& edge) {
        m_links.emplace_back(v.index(), edge);
    }

   public:
    using edge_type = E;

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;
    using edges_iterator = Edges_iterator<false>;
    using const_edges_iterator = Edges_iterator<true>;

    inline size_t index() const {
        return derived() - m_adjacency_lists->m_vertices.cbegin();
    }
    inline operator size_t() const { return index(); }

    iterator begin() { return {derived(), m_links.begin()}; }
    iterator end() { return {derived(), m_links.end()}; }
    const_iterator cbegin() const { return {derived(), m_links.cbegin()}; }
    const_iterator cend() const { return {derived(), m_links.cend()}; }

    edges_iterator edges_begin() { return {derived(), m_links.begin()}; }
    edges_iterator edges_end() { return {derived(), m_links.end()}; }
    const_edges_iterator cedges_begin() const {
        return {derived(), m_links.cbegin()};
    }
    const_edges_iterator cedges_end() const {
        return {derived(), m_links.cend()};
    }

    void remove_edge(const AdjListsVertexBase& v) {
        m_links.remove_first_if([&v](const vertex_link_type& edge) {
            return v.index() == edge.target();
        });
    }
    bool has_edge(const AdjListsVertexBase& v) const {
        for (auto l = cbegin(); l != cend(); ++l)
            if (*l == v) return true;
        return false;
    }
    const E* get_edge(size_t v) const {
        for (auto e = cedges_begin(); e != cedges_end(); ++e)
            if (e->target().index() == v) return e->m_edge;
        return nullptr;
    }
    E* get_edge(size_t v) {
        for (auto e = edges_begin(); e != edges_end(); ++e)
            if (e->target().index() == v) return e->m_edge;
        return nullptr;
    }
};

template <GraphType T_graph_type, typename VT, typename E>
class Vertex : public AdjListsVertexBase<T_graph_type, VT, E,
                                         Vertex<T_graph_type, VT, E>> {
   private:
    using Base =
        AdjListsVertexBase<T_graph_type, VT, E, Vertex<T_graph_type, VT, E>>;
    template <GraphType TT_graph_type, typename VV>
    friend class AdjacencyListsBase;
    template <GraphType TT_graph_type, typename VV, typename ET>
    friend class AdjacencyLists;
    friend class Vector<Vertex>;

    Vertex(const VT& value, typename Base::adj_lists_type* adjacency_lists)
        : Base(value, adjacency_lists) {}
    Vertex() : Base() {}

    Vertex(const Vertex&) = default;
    Vertex& operator=(const Vertex&) = default;
    Vertex(Vertex&&) = default;
    Vertex& operator=(Vertex&&) = default;

   public:
    using edge_type = E;
};

template <GraphType T_graph_type, typename V>
class AdjacencyListsBase {
   public:
    using vertex_type = V;
    using edge_type = typename V::edge_type;

   protected:
    Vector<vertex_type> m_vertices;

   private:
    template <GraphType TT_graph_type, typename VV, typename EE, typename D>
    friend class AdjListsVertexBase;
    void update_vertices_this_link() {
        for (auto& v : m_vertices) v.m_adjacency_lists = this;
    }

   public:
    AdjacencyListsBase() = default;

    AdjacencyListsBase(const AdjacencyListsBase& o, bool update_links)
        : m_vertices(o.m_vertices) {}
    AdjacencyListsBase(const AdjacencyListsBase& o)
        : AdjacencyListsBase(o, true) {
        update_vertices_this_link();
    }
    AdjacencyListsBase& operator=(const AdjacencyListsBase& o) {
        auto copy = o;
        std::swap(*this, copy);
        return *this;
    }
    AdjacencyListsBase(AdjacencyListsBase&& o)
        : m_vertices(std::move(o.m_vertices)) {
        update_vertices_this_link();
    }
    AdjacencyListsBase& operator=(AdjacencyListsBase&& o) {
        std::swap(m_vertices, o.m_vertices);
        update_vertices_this_link();
        o.update_vertices_this_link();
        return *this;
    }

    vertex_type& create_vertex(const typename V::value_type& t) {
        m_vertices.emplace_back(t, this);
        return m_vertices[m_vertices.size() - 1];
    }

    size_t vertices_count() const { return m_vertices.size(); }

    const vertex_type& operator[](size_t index) const {
        return m_vertices[index];
    }
    vertex_type& operator[](size_t index) { return m_vertices[index]; }

    bool has_edge(const vertex_type& v, const vertex_type& w) const {
        return v.has_edge(w);
    }

    edge_type* get_edge(size_t v, size_t w) {
        return m_vertices[v].get_edge(w);
    }

    const edge_type* get_edge(const vertex_type& v,
                              const vertex_type& w) const {
        return v.get_edge(w);
    }
    edge_type* get_edge(vertex_type& v, const vertex_type& w) {
        return v.get_edge(w);
    }

    auto cbegin() const { return m_vertices.cbegin(); }
    auto cend() const { return m_vertices.cend(); }
    auto begin() { return m_vertices.begin(); }
    auto end() { return m_vertices.end(); }

    auto crbegin() const { return m_vertices.crbegin(); }
    auto crend() const { return m_vertices.crend(); }
};

template <GraphType T_graph_type, typename V>
struct EdgesRemover {
    void remove_edge(V& v1, V& v2) {
        v1.remove_edge(v2);
        v2.remove_edge(v1);
    }
};

template <typename V>
struct EdgesRemover<GraphType::DIGRAPH, V> {
    void remove_edge(V& v1, V& v2) { v1.remove_edge(v2); }
};

template <GraphType T_graph_type, typename V,
          typename ET = typename V::edge_type::value_type>
class AdjacencyLists : public AdjacencyListsBase<T_graph_type, V>,
                       public EdgesRemover<T_graph_type, V> {
   public:
    AdjacencyLists& add_edge(V& v1, V& v2, const typename V::edge_type& edge) {
        if (!v1.link_exists(v2)) {
            v1.add_link(v2, edge);
            v2.add_link(v1, edge);
        }
        return *this;
    }
};

template <typename V>
class AdjacencyLists<GraphType::GRAPH, V, bool>
    : public AdjacencyListsBase<GraphType::GRAPH, V>,
      public EdgesRemover<GraphType::GRAPH, V> {
   public:
    AdjacencyLists& add_edge(V& v1, V& v2) {
        if (!v1.link_exists(v2)) {
            v1.add_link(v2, true);
            v2.add_link(v1, true);
        }
        return *this;
    }
};

template <typename V, typename ET>
class AdjacencyLists<GraphType::DIGRAPH, V, ET>
    : public AdjacencyListsBase<GraphType::DIGRAPH, V>,
      public EdgesRemover<GraphType::DIGRAPH, V> {
   public:
    AdjacencyLists& add_edge(V& v1, V& v2, const typename V::edge_type& edge) {
        if (!v1.link_exists(v2)) v1.add_link(v2, edge);
        return *this;
    }
};

template <typename V>
class AdjacencyLists<GraphType::DIGRAPH, V, bool>
    : public AdjacencyListsBase<GraphType::DIGRAPH, V>,
      public EdgesRemover<GraphType::DIGRAPH, V> {
   public:
    AdjacencyLists& add_edge(V& v1, V& v2) {
        if (!v1.link_exists(v2)) v1.add_link(v2, true);
        return *this;
    }
};

template <GraphType T_graph_type, typename VT, typename E, typename D>
template <bool T_is_const>
class AdjListsVertexBase<T_graph_type, VT, E, D>::Iterator {
   private:
    using value_type = std::conditional_t<T_is_const, const D, D>;

   protected:
    using link_type = VertexLink<T_graph_type, E>;
    using links_type = Forward_list<link_type>;
    using links_iterator_type =
        std::conditional_t<T_is_const, typename links_type::const_iterator,
                           typename links_type::iterator>;
    value_type* m_vertex;

   private:
    friend AdjListsVertexBase;  // AdjLists_vertex_base;
    links_iterator_type m_it;
    Iterator(value_type* vertex, const links_iterator_type& it)
        : m_vertex(vertex), m_it(it) {}

   public:
    const Iterator& operator++() {
        ++m_it;
        return *this;
    }
    bool operator==(const Iterator& o) const { return m_it == o.m_it; }
    bool operator!=(const Iterator& o) const { return !operator==(o); }
    value_type* operator->() const { return &operator*(); }
    value_type& operator*() const {
        return m_vertex->m_adjacency_lists->m_vertices[m_it->target()];
    }
};

template <GraphType T_graph_type, typename VT, typename E, typename D>
template <bool T_is_const>
class AdjListsVertexBase<T_graph_type, VT, E, D>::Edges_iterator
    : public Iterator<T_is_const> {
   private:
    using Base = Iterator<T_is_const>;
    using link_type = typename Base::link_type;

   public:
    using entry_type = EdgesIteratorEntry<D, link_type, T_is_const>;

   private:
    using vertex_base_type = typename Base::value_type;
    using links_type = typename Base::links_type;
    using links_iterator_type = typename Base::links_iterator_type;

    entry_type m_entry;

    static auto links_end(Forward_list<link_type>& links) {
        return links.end();
    }
    static auto links_end(const Forward_list<link_type>& links) {
        return links.cend();
    }

    template <typename VV, typename EE, bool TT_is_const, typename It>
    static void update_edge_p(EdgesIteratorEntry<VV, EE, TT_is_const>& entry,
                              const It& it) {
        entry.m_edge = &*it;
    }

    void update_entry() {
        if (Base::m_it != links_end(Base::m_vertex->m_links)) {
            // todo target_ is redundant, delete
            m_entry.m_target = &Base::m_vertex->m_adjacency_lists
                                    ->m_vertices[Base::m_it->target()];
            update_edge_p(m_entry, Base::m_it);
        }
    }

   public:
    Edges_iterator(vertex_base_type* vertex, const links_iterator_type& it)
        : Base(vertex, it), m_entry(vertex) {
        update_entry();
    }
    const entry_type& operator*() const { return m_entry; }
    const entry_type* operator->() const { return &m_entry; }
    Edges_iterator& operator++() {
        Base::operator++();
        update_entry();
        return *this;
    }
};

template <typename G, typename D>
struct InternalPrinterBase {
    static void print(const G& g, std::ostream& stream) {
        for (auto v = g.cbegin(); v != g.cend(); ++v) {
            stream << *v << ": ";
            D::print_vertex(*v, stream);
            stream << std::endl;
        }
    }
};

template <GraphType TT_graph_type, typename VV, typename EE>
struct InternalPrinter
    : public InternalPrinterBase<
          AdjacencyLists<TT_graph_type, Vertex<TT_graph_type, VV, EE>>,
          InternalPrinter<TT_graph_type, VV, EE>> {
    static void print_vertex(const Vertex<TT_graph_type, VV, EE>& v,
                             std::ostream& stream) {
        for (auto w = v.cedges_begin(); w != v.cedges_end(); ++w)
            stream << w->target().index() << "(" << w->edge().weight() << ") ";
    }
};

template <GraphType TT_graph_type, typename VV>
struct InternalPrinter<TT_graph_type, VV, Edge<bool>>
    : public InternalPrinterBase<
          AdjacencyLists<TT_graph_type, Vertex<TT_graph_type, VV, Edge<bool>>>,
          InternalPrinter<TT_graph_type, VV, Edge<bool>>> {
    static void print_vertex(const Vertex<TT_graph_type, VV, Edge<bool>>& v,
                             std::ostream& stream) {
        for (auto w = v.cedges_begin(); w != v.cedges_end(); ++w)
            stream << w->target().index() << " ";
    }
};

}  // namespace Adjacency_lists_ns

template <GraphType T_graph_type, typename V, typename TE = bool,
          typename E = Adjacency_lists_ns::Edge<TE>>
using AdjacencyLists = Adjacency_lists_ns::AdjacencyLists<
    T_graph_type, Adjacency_lists_ns::Vertex<T_graph_type, V, E>>;

template <GraphType T_graph_type, typename V>
void print_representation(
    const Adjacency_lists_ns::AdjacencyLists<T_graph_type, V>& g,
    std::ostream& stream) {
    Adjacency_lists_ns::InternalPrinter<T_graph_type, typename V::value_type,
                                        typename V::edge_type>::print(g,
                                                                      stream);
}

}  // namespace Graph
