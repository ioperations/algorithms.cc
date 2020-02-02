#pragma once

#include <iostream>

#include "graph.h"
#include "vector"
#include "forward_list.h"

namespace Graph {

    namespace Adjacency_lists_ns {

        template<typename T>
            class Edge {
                private:
                    T weight_;
                public:
                    using value_type = T; // rename to weight_type?
                    Edge(const T& t) :weight_(t) {}
                    T weight() const { return weight_; }
                    void set_weight(const T& weight) { weight_ = weight; }
            };
        // todo Edge<bool> specialization

        template<typename V, typename E>
            class Adjacency_lists_base {
                private:
                    class Vertex;
                    Vector<Vertex> vertices_;
                    void update_vertices_this_link() {
                        for (auto& v : vertices_)
                            v.adjacency_lists_ = this;
                    }
                protected:
                    using vertex_type = Vertex;
                public:
                    using value_type = V;
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

                    Vertex& create_vertex(const V& t) {
                        vertices_.push_back(Vertex(t, this));
                        return vertices_[vertices_.size() - 1];
                    }
                    size_t vertices_count() const {
                        return vertices_.size();
                    }
                    const Vertex& vertex_at(size_t index) const {
                        return vertices_[index];
                    }
                    Vertex& vertex_at(size_t index) {
                        return vertices_[index];
                    }
                    bool has_edge(const Vertex& v, const Vertex& w) const {
                        return v.has_edge(w);
                    }

                    auto cbegin() const { return vertices_.cbegin(); }
                    auto cend() const { return vertices_.cend(); }
                    auto begin() { return vertices_.begin(); }
                    auto end() { return vertices_.end(); } 

                    auto crbegin() const { return vertices_.crbegin(); }
                    auto crend() const { return vertices_.crend(); }

                    void print_internal(std::ostream& stream) {
                        for (auto& v : vertices_) {
                            stream << v.index() << ": ";
                            for (auto& w : v)
                                stream << w.index() << " ";
                            stream << std::endl;
                        }
                    }
            };

        template<Graph_type T_graph_type, typename V, typename E = Edge<bool>>
            class Adjacency_lists : public Adjacency_lists_base<V, E> {
                private:
                    using Base = Adjacency_lists_base<V, E>;
                    template<Graph_type TT_graph_type, typename VV, typename EE>
                        struct Base_edges_handler {};
                    template<typename VV, typename EE>
                        struct Base_edges_handler<Graph_type::GRAPH, VV, EE> {
                            void add_edge(VV& v1, VV& v2, const E& edge) {
                                v1.add_link(v2, edge);
                                v2.add_link(v1, edge);
                            }
                            void remove_edge(VV& v1, VV& v2) {
                                v1.remove_edge(v2);
                                v2.remove_edge(v1);
                            }
                        };
                    template<typename VV, typename EE>
                        struct Base_edges_handler<Graph_type::DIGRAPH, VV, EE> {
                            void add_edge(VV& v1, VV& v2, const E& edge) {
                                v1.add_link(v2, edge);
                            }
                            void remove_edge(VV& v1, VV& v2) {
                                v1.remove_edge(v2);
                            }
                        };

                    template<Graph_type TT_graph_type, typename VV, typename EE>
                        struct Edges_handler : public Base_edges_handler<TT_graph_type, VV, EE> {};
                    template<Graph_type TT_graph_type, typename VV>
                        struct Edges_handler<TT_graph_type, VV, Edge<bool>> 
                        : public Base_edges_handler<TT_graph_type, VV, Edge<bool>> {
                            void add_edge(VV& v1, VV& v2) {
                                Base_edges_handler<TT_graph_type, VV, Edge<bool>>::add_edge(v1, v2, true);
                            }
                        };
                public:
                    using vertex_type = typename Base::vertex_type;
                    Edges_handler<T_graph_type, vertex_type, E> edges_handler_;
                    Adjacency_lists& add_edge(vertex_type& v1, vertex_type& v2, const E& edge) { // todo rename to set_edge
                        edges_handler_.add_edge(v1, v2, edge);
                        return *this;
                    }
                    Adjacency_lists& add_edge(vertex_type& v1, vertex_type& v2) { // todo rename to set_edge
                        edges_handler_.add_edge(v1, v2);
                        return *this;
                    }
                    void remove_edge(vertex_type& v1, vertex_type& v2) {
                        edges_handler_.remove_edge(v1, v2);
                    }
            };

        template<typename V, typename E>
            class Adjacency_lists_base<V, E>::Vertex : public Vertex_base<V> {
                private:
                    class Link;
                    template<bool T_is_const>
                        class Iterator;
                    template<bool T_is_const>
                        class Edges_iterator;

                    friend class Adjacency_lists_base;
                    template<Graph_type graph_type, typename VV, typename EE>
                        friend class Adjacency_lists;
                    friend class Vector<Vertex>;

                    Adjacency_lists_base* adjacency_lists_;
                    Forward_list<Link> links_;

                    Vertex(V value, Adjacency_lists_base* adjacency_lists)
                        :Vertex_base<V>(value), adjacency_lists_(adjacency_lists)
                    {}
                    Vertex() :adjacency_lists_(nullptr) {}

                    void add_link(const Vertex& v, const E& edge) { // todo rename?
                        bool found = false;
                        for (auto link = links_.begin(); link != links_.end() && !found; ++link)
                            found = link->target_ == v.index();
                        if (!found)
                            links_.emplace_back(v.index(), edge); // todo hardcoded
                    }
                public:
                    using iterator = Iterator<false>;
                    using const_iterator = Iterator<true>;
                    using edges_iterator = Edges_iterator<false>;

                    size_t index() const { return this - adjacency_lists_->vertices_.cbegin(); }
                    operator size_t() const { return index(); }

                    iterator begin() { return {this, links_.begin()}; }
                    iterator end() { return {this, links_.end()}; }
                    const_iterator cbegin() const { return {this, links_.cbegin()}; }
                    const_iterator cend() const { return {this, links_.cend()}; }

                    edges_iterator edges_begin() { return {this, links_.begin()}; }
                    edges_iterator edges_end() { return {this, links_.end()}; }

                    void remove_edge(const Vertex& v) {
                        links_.remove_first_if([&v](const Link& edge) { return v.index() == edge.target_; });
                    }
                    bool has_edge(const Vertex& v) const {
                        for (auto l = v.cbegin(); l != v.cend(); ++l)
                            if (*l == v)
                                return true;
                        return false;
                    }
            };

        template<typename V, typename E>
            class Adjacency_lists_base<V, E>::Vertex::Link : public E { // todo const / non-const versions?
                public:
                    size_t target_;
                    Link(size_t target, const E& edge) :E(edge), target_(target) {}
            };

        template<typename V, typename E>
            template<bool T_is_const>
            class Adjacency_lists_base<V, E>::Vertex::Iterator {
                protected:
                    using link_type = typename Vertex::Link;
                    using links_type = Forward_list<link_type>;
                    using links_iterator_type = std::conditional_t<T_is_const,
                          typename links_type::const_iterator, typename links_type::iterator>;
                    using value_type = std::conditional_t<T_is_const, const Vertex, Vertex>;

                    value_type* vertex_;
                private:
                    friend class Vertex;
                    links_iterator_type it_;
                    Iterator(value_type* vertex, const links_iterator_type& it)
                        :vertex_(vertex), it_(it)
                    {}
                public:
                    const Iterator& operator++() {
                        ++it_;
                        return *this;
                    }
                    bool operator==(const Iterator& o) const {
                        return it_ == o.it_;
                    }
                    bool operator!=(const Iterator& o) const {
                        return !operator==(o);
                    }
                    value_type* operator->() const {
                        return &operator*();
                    }
                    value_type& operator*() const {
                        return vertex_->adjacency_lists_->vertices_[it_->target_];
                    }
            };

        template<typename V, typename E>
            template<bool T_is_const>
            class Adjacency_lists_base<V, E>::Vertex::Edges_iterator : public Iterator<T_is_const> {
                private:
                    using Base = Iterator<T_is_const>;
                    using vertex_type = std::conditional_t<T_is_const, const Vertex, Vertex>;
                    using links_type = typename Base::links_type;
                    using links_iterator_type = typename Base::links_iterator_type;
                    using entry_type = Edges_iterator_entry<Vertex, E, T_is_const>;

                    entry_type entry_;

                    void update_entry() {
                        if (Base::it_ != Base::vertex_->links_.end()) {
                            entry_.target_ = &Base::vertex_->adjacency_lists_->vertices_[Base::it_->target_];
                            entry_.edge_ = &*Base::it_;
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

    template<Graph_type graph_type, typename V, typename E = Adjacency_lists_ns::Edge<bool>>
        using Adjacency_lists = Adjacency_lists_ns::Adjacency_lists<graph_type, V, E>;

    template<Graph_type T_type, typename VV, typename EE = bool>
        Adjacency_lists<T_type, VV, Adjacency_lists_ns::Edge<EE>> create_adj_lists() { return {}; }
}
