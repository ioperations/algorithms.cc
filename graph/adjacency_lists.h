#pragma once

#include <iostream>

#include "graph_common.h"
#include "vector.h"
#include "forward_list.h"

namespace Graph {

    namespace Adjacency_lists_ns {

        template<typename T>
            class Edge {
                private:
                    T weight_;
                public:
                    using value_type = T;
                    Edge(const T& t) :weight_(t) {}
                    T weight() const { return weight_; }
                    void set_weight(const T& weight) { weight_ = weight; }
            };

        template<typename E>
            class Vertex_link : public E {
                public:
                    size_t target_;
                    Vertex_link(size_t target, const E& edge) :E(edge), target_(target) {}
            };

        template<>
            class Vertex_link<Edge<bool>> {
                public:
                    size_t target_;
                    Vertex_link(size_t target, const Edge<bool>& edge) :target_(target) {}
            };

        template<Graph_type TT_graph_type, typename VV, typename EE>
            class Adjacency_lists_base;

        template<Graph_type T_graph_type, typename V, typename E>
            class Adj_lists_vertex_base : public Vertex_base<V> {
                private:
                    template<bool T_is_const>
                        class Iterator;
                    template<bool T_is_const>
                        class Edges_iterator;

                    template<Graph_type graph_type, typename VV, typename EE, typename EET>
                        friend class Adjacency_lists;

                    using adj_lists_type = Adjacency_lists_base<T_graph_type, V, E>;

                    adj_lists_type* adjacency_lists_;
                    Forward_list<Vertex_link<E>> links_;

                    void add_link(const Adj_lists_vertex_base& v, const E& edge) {
                        bool found = false;
                        for (auto link = links_.begin(); link != links_.end() && !found; ++link)
                            found = link->target_ == v.index();
                        if (!found)
                            links_.emplace_back(v.index(), edge);
                    }
                protected:
                    Adj_lists_vertex_base(V value, adj_lists_type* adjacency_lists)
                        :Vertex_base<V>(value), adjacency_lists_(adjacency_lists)
                    {}
                    Adj_lists_vertex_base() :adjacency_lists_(nullptr) {}
                public:
                    using iterator = Iterator<false>;
                    using const_iterator = Iterator<true>;
                    using edges_iterator = Edges_iterator<false>;
                    using const_edges_iterator = Edges_iterator<true>;

                    size_t index() const { 
                        return 1;
                        // return this - adjacency_lists_->vertices_.cbegin(); 
                        // todo compare child and parent this pointers
                    }
                    operator size_t() const { return index(); }

                    iterator begin() { return {this, links_.begin()}; }
                    iterator end() { return {this, links_.end()}; }
                    const_iterator cbegin() const { return {this, links_.cbegin()}; }
                    const_iterator cend() const { return {this, links_.cend()}; }

                    edges_iterator edges_begin() { return {this, links_.begin()}; }
                    edges_iterator edges_end() { return {this, links_.end()}; }
                    const_edges_iterator cedges_begin() const { return {this, links_.cbegin()}; }
                    const_edges_iterator cedges_end() const { return {this, links_.cend()}; }

                    void remove_edge(const Adj_lists_vertex_base& v) {
                        links_.remove_first_if([&v](const Vertex_link<E>& edge) { return v.index() == edge.target_; });
                    }
                    bool has_edge(const Adj_lists_vertex_base& v) const {
                        for (auto l = cbegin(); l != cend(); ++l)
                            if (*l == v)
                                return true;
                        return false;
                    }
                    const E* get_edge(const Adj_lists_vertex_base& v) const {
                        for (auto e = cedges_begin(); e != cedges_end(); ++e)
                            if (e->target() == v)
                                return e->edge_;
                        return nullptr;
                    }
                    E* get_edge(const Adj_lists_vertex_base& v) {
                        for (auto e = edges_begin(); e != edges_end(); ++e)
                            if (e->target() == v)
                                return e->edge_;
                        return nullptr;
                    }
            };

        template<Graph_type T_graph_type, typename V, typename E>
            class Vertex : public Adj_lists_vertex_base<T_graph_type, V, E> {
                private:
                    using Base = Adj_lists_vertex_base<T_graph_type, V, E>;
                    using adj_lists_type = Adjacency_lists_base<T_graph_type, V, E>;
                    template<Graph_type TT_graph_type, typename VV, typename EE>
                        friend class Adjacency_lists_base;
                    friend class Vector<Vertex>;
                    Vertex(V value, adj_lists_type* adjacency_lists)
                        :Base(value, adjacency_lists)
                    {}
                    Vertex() :Base() {}
            };

        template<Graph_type T_graph_type, typename V, typename E>
            class Adjacency_lists_base {
                public:
                    using vertex_type = Vertex<T_graph_type, V, E>;
                private:
                    template<Graph_type TT_graph_type, typename VV, typename EE>
                        friend class Adj_lists_vertex_base;
                    Vector<vertex_type> vertices_;
                    void update_vertices_this_link() {
                        for (auto& v : vertices_)
                            v.adjacency_lists_ = this;
                    }
                    template<Graph_type TT_graph_type, typename VV, typename EE>
                        struct Internal_printer {
                            static void print(const Vertex<TT_graph_type, VV, EE>& v,
                                              std::ostream& stream) {
                                for (auto w = v.cedges_begin(); w != v.cedges_end(); ++w)
                                    stream << w->target().index() << "(" << w->edge().weight() << ") ";
                            }
                        };
                    template<Graph_type TT_graph_type, typename VV>
                        struct Internal_printer<TT_graph_type, VV, Edge<bool>> {
                            static void print(const Vertex<TT_graph_type, VV, Edge<bool>>& v,
                                              std::ostream& stream) {
                                for (auto w = v.cbegin(); w != v.cend(); ++w)
                                    stream << w->index() << " ";
                            }
                        };
                public:
                    using value_type = V;
                    using edge_type = E;
                    Adjacency_lists_base() = default;
                    Adjacency_lists_base(const Adjacency_lists_base& o) :vertices_(o.vertices_) {
                        update_vertices_this_link();
                    }
                    Adjacency_lists_base& operator=(const Adjacency_lists_base& o) {
                        auto copy = o;
                        std::swap(*this, copy);
                        return *this;
                    }
                    Adjacency_lists_base(Adjacency_lists_base&& o) :vertices_(std::move(o.vertices_)) {
                        update_vertices_this_link();
                    }
                    Adjacency_lists_base& operator=(Adjacency_lists_base&& o) {
                        std::swap(vertices_, o.vertices_);
                        update_vertices_this_link();
                        return *this;
                    }

                    vertex_type& create_vertex(const V& t) {
                        vertices_.push_back(vertex_type(t, this));
                        return vertices_[vertices_.size() - 1];
                    }

                    size_t vertices_count() const { return vertices_.size(); }
                    const vertex_type& vertex_at(size_t index) const { return vertices_[index]; }
                    vertex_type& vertex_at(size_t index) { return vertices_[index]; }

                    bool has_edge(const vertex_type& v, const vertex_type& w) const { return v.has_edge(w); }

                    const E* get_edge(const vertex_type& v, const vertex_type& w) const { return v.get_edge(w); }
                    E* get_edge(vertex_type& v, const vertex_type& w) { return v.get_edge(w); }

                    auto cbegin() const { return vertices_.cbegin(); }
                    auto cend() const { return vertices_.cend(); }
                    auto begin() { return vertices_.begin(); }
                    auto end() { return vertices_.end(); } 

                    auto crbegin() const { return vertices_.crbegin(); }
                    auto crend() const { return vertices_.crend(); }

                    void print_internal(std::ostream& stream) {
                        for (auto& v : vertices_) {
                            stream << v.index() << ": ";
                            Internal_printer<T_graph_type, V, E>::print(v, stream);
                            stream << std::endl;
                        }
                    }
            };

        template<Graph_type T_graph_type, typename V, typename E>
            struct Edges_remover {
                using vertex_type = typename Adjacency_lists_base<T_graph_type, V, E>::vertex_type;
                void remove_edge(vertex_type& v1, vertex_type& v2) {
                    v1.remove_edge(v2);
                    v2.remove_edge(v1);
                }
            };

        template<typename V, typename E>
            struct Edges_remover<Graph_type::DIGRAPH, V, E> {
                using vertex_type = typename Adjacency_lists_base<Graph_type::DIGRAPH, V, E>::vertex_type;
                void remove_edge(vertex_type& v1, vertex_type& v2) {
                    v1.remove_edge(v2);
                }
            };

        template<Graph_type T_graph_type, typename V, typename E, typename ET = typename E::value_type>
            class Adjacency_lists : public Adjacency_lists_base<T_graph_type, V, E>, public Edges_remover<T_graph_type, V, E> {
                public:
                    using vertex_type = typename Adjacency_lists_base<T_graph_type, V, E>::vertex_type;
                    Adjacency_lists& add_edge(vertex_type& v1, vertex_type& v2, const E& edge) {
                        std::cout << "add graph edge" << std::endl;
                        v1.add_link(v2, edge);
                        v2.add_link(v1, edge);
                        return *this;
                    }
            };

        template<typename V, typename E>
            class Adjacency_lists<Graph_type::GRAPH, V, E, bool>
            : public Adjacency_lists_base<Graph_type::GRAPH, V, E>, public Edges_remover<Graph_type::GRAPH, V, E> {
                public:
                    using vertex_type = typename Adjacency_lists_base<Graph_type::GRAPH, V, E>::vertex_type;
                    Adjacency_lists& add_edge(vertex_type& v1, vertex_type& v2) {
                        v1.add_link(v2, true);
                        v2.add_link(v1, true);
                        return *this;
                    }
            };

        template<typename V, typename E, typename ET>
            class Adjacency_lists<Graph_type::DIGRAPH, V, E, ET> 
            : public Adjacency_lists_base<Graph_type::DIGRAPH, V, E>, public Edges_remover<Graph_type::DIGRAPH, V, E> {
                public:
                    using vertex_type = typename Adjacency_lists_base<Graph_type::DIGRAPH, V, E>::vertex_type;
                    Adjacency_lists& add_edge(vertex_type& v1, vertex_type& v2, const E& edge) {
                        v1.add_link(v2, edge);
                        return *this;
                    }
            };

        template<typename V, typename E>
            class Adjacency_lists<Graph_type::DIGRAPH, V, E, bool>
            : public Adjacency_lists_base<Graph_type::DIGRAPH, V, E>, public Edges_remover<Graph_type::DIGRAPH, V, E> {
                public:
                    using vertex_type = typename Adjacency_lists_base<Graph_type::DIGRAPH, V, E>::vertex_type;
                    Adjacency_lists& add_edge(vertex_type& v1, vertex_type& v2) {
                        v1.add_link(v2, true);
                        return *this;
                    }
            };

        template<typename V, typename E, typename ET>
            class Adjacency_lists<Graph_type::FLOW, V, E, ET> 
            : public Adjacency_lists_base<Graph_type::FLOW, V, E>, public Edges_remover<Graph_type::FLOW, V, E> {
                public:
                    using vertex_type = typename Adjacency_lists_base<Graph_type::FLOW, V, E>::vertex_type;
                    Adjacency_lists& add_edge(vertex_type& v1, vertex_type& v2, const E& edge) {
                        std::cout << "add flow edge" << std::endl;
                        v1.add_link(v2, edge);
                        v2.add_link(v1, edge);
                        return *this;
                    }
            };

        template<Graph_type T_graph_type, typename V, typename E>
            template<bool T_is_const>
            class Adj_lists_vertex_base<T_graph_type, V, E>::Iterator {
                protected:
                    using link_type = Vertex_link<E>;
                    using links_type = Forward_list<link_type>;
                    using links_iterator_type = std::conditional_t<T_is_const,
                          typename links_type::const_iterator, typename links_type::iterator>;
                    using value_type = std::conditional_t<T_is_const, const Adj_lists_vertex_base, Adj_lists_vertex_base>;

                    value_type* vertex_;
                private:
                    friend class Adj_lists_vertex_base;
                    links_iterator_type it_;
                    Iterator(value_type* vertex, const links_iterator_type& it)
                        :vertex_(vertex), it_(it)
                    {}
                public:
                    const Iterator& operator++() {
                        ++it_;
                        return *this;
                    }
                    bool operator==(const Iterator& o) const { return it_ == o.it_; }
                    bool operator!=(const Iterator& o) const { return !operator==(o); }
                    value_type* operator->() const { return &operator*(); }
                    value_type& operator*() const {
                        return vertex_->adjacency_lists_->vertices_[it_->target_];
                    }
            };

        template<Graph_type T_graph_type, typename V, typename E>
            template<bool T_is_const>
            class Adj_lists_vertex_base<T_graph_type, V, E>::Edges_iterator : public Iterator<T_is_const> {
                public:
                    using entry_type = Edges_iterator_entry<Adj_lists_vertex_base, E, T_is_const>;
                private:
                    using Base = Iterator<T_is_const>;
                    using vertex_type = std::conditional_t<T_is_const, const Adj_lists_vertex_base, Adj_lists_vertex_base>;
                    using link_type = typename Base::link_type;
                    using links_type = typename Base::links_type;
                    using links_iterator_type = typename Base::links_iterator_type;

                    entry_type entry_;

                    static auto links_end(Forward_list<link_type>& links) { return links.end(); }
                    static auto links_end(const Forward_list<link_type>& links) { return links.cend(); }

                    template<typename VV, typename EE, bool TT_is_const, typename It>
                        static void update_edge_p(Edges_iterator_entry<VV, EE, TT_is_const>& entry, const It& it) {
                            entry.edge_ = &*it;
                        }
                    template<typename VV, bool TT_is_const, typename It>
                        static void update_edge_p(Edges_iterator_entry<VV, Edge<bool>, TT_is_const>& entry,
                                                  const It& it) {}

                    void update_entry() {
                        if (Base::it_ != links_end(Base::vertex_->links_)) {
                            entry_.target_ = &Base::vertex_->adjacency_lists_->vertices_[Base::it_->target_];
                            update_edge_p(entry_, Base::it_);
                        }
                    }
                public:
                    Edges_iterator(vertex_type* vertex, const links_iterator_type& it)
                        :Base(vertex, it), entry_(vertex)
                    { update_entry(); }
                    const entry_type& operator*() const { return entry_; }
                    const entry_type* operator->() const { return &entry_; }
                    Edges_iterator& operator++() { 
                        Base::operator++();
                        update_entry();
                        return *this;
                    }
            };

    }

    template<Graph_type graph_type, typename V, typename TE = bool, typename E = Adjacency_lists_ns::Edge<TE>>
        using Adjacency_lists = Adjacency_lists_ns::Adjacency_lists<graph_type, V, E>;

}
