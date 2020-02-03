#pragma once

#include <iostream>

#include "graph.h"
#include "vector.h"

namespace Graph {

    namespace Adjacency_matrix_ns {

        template<typename T>
            class Edge {
                private:
                    T weight_;
                public:
                    static constexpr T empty_value() { return -1; }
                    using value_type = T;
                    Edge() :weight_(empty_value()) {}
                    Edge(const T& t) :weight_(t) {}
                    T weight() const { return weight_; }
                    void set_weight(const T& weight) { weight_ = weight; }
                    bool exists() const { return weight_ != empty_value(); }
                    operator bool() const { return exists(); }
            };

        template<>
            bool Edge<bool>::empty_value() {
                return false;
            }

        template<typename V, typename E>
            class Adjacency_matrix_base {
                private:
                    class Vertex;
                    friend class Vertex;
                    Vector<Vertex> vertices_;
                    void update_vertices_this_link() {
                        for (auto& v : vertices_)
                            v.matrix_ = this;
                    }
                protected:
                    using vertex_type = Vertex;
                    Vector<Vector<E>> edges_;
                public:
                    using value_type = V;

                    Adjacency_matrix_base() :edges_(100) {
                        for (auto& l : edges_) l = Vector<E>(100);
                    }

                    Adjacency_matrix_base(const Adjacency_matrix_base& o) :vertices_(o.vertices_), edges_(o.edges_) {
                        update_vertices_this_link();
                    }
                    Adjacency_matrix_base& operator=(const Adjacency_matrix_base& o) {
                        auto copy = o;
                        std::swap(*this, copy);
                        return *this;
                    }
                    Adjacency_matrix_base(Adjacency_matrix_base&& o) 
                        :vertices_(std::move(o.vertices_)), edges_(std::move(o.edges_)) {
                            update_vertices_this_link();
                        }
                    Adjacency_matrix_base& operator=(Adjacency_matrix_base&& o) {
                        std::swap(vertices_, o.vertices_);
                        std::swap(edges_, o.edges_);
                        update_vertices_this_link();
                        return *this;
                    }

                    vertex_type& create_vertex(const V& t) {
                        vertices_.push_back(vertex_type(t, this)); // todo add emplace back method
                        return vertices_[vertices_.size() - 1];
                    }
                    size_t vertices_count() const {
                        return vertices_.size();
                    }
                    const vertex_type& vertex_at(size_t index) const {
                        return vertices_[index];
                    }
                    vertex_type& vertex_at(size_t index) {
                        return vertices_[index];
                    }
                    bool has_edge(const vertex_type& v, const vertex_type& w) {
                        return edges_[v][w];
                    }
                    E* get_edge(const vertex_type& v, const vertex_type& w) {
                        auto& edge = edges_[v][w];
                        return edge.exists() ? &edge : nullptr;
                    }

                    auto begin() { return vertices_.begin(); }
                    auto end() { return vertices_.end(); }
                    auto cbegin() const { return vertices_.cbegin(); }
                    auto cend() const { return vertices_.cend(); }
                    auto crbegin() const { return vertices_.crbegin(); }
                    auto crend() const { return vertices_.crend(); }

                    void print_internal(std::ostream& stream) const {
                        auto size = vertices_.size();
                        for (size_t r = 0; r < size; ++r) {
                            for (size_t c = 0; c < size; ++c) {
                                stream << edges_[r][c] << " ";
                            }
                            stream << std::endl;
                        }
                    }
            };

    template<Graph_type graph_type, typename V, typename E = Edge<bool>>
        class Adjacency_matrix : public Adjacency_matrix_base<V, E> {
            public:
                using vertex_type = typename Adjacency_matrix_base<V, E>::vertex_type;
            private:
                template<typename TT, typename EE>
                    static void set_edge(Adjacency_matrix<Graph_type::GRAPH, TT, EE>& m,
                                         const vertex_type& v1, const vertex_type& v2, const E& e) {
                        m.edges_[v1][v2] = e;
                        m.edges_[v2][v1] = e;
                    }
                template<typename TT, typename EE>
                    static void set_edge(Adjacency_matrix<Graph_type::DIGRAPH, TT, EE>& m,
                                         const vertex_type& v1, const vertex_type& v2, const E& e) {
                        m.edges_[v1][v2] = e;
                    }
                template<Graph_type gt, typename TT>
                    static void set_edge(Adjacency_matrix<gt, TT, Edge<bool>>& m,
                                         const vertex_type& v1, const vertex_type& v2) {
                        set_edge(m, v1, v2, {true});
                    }
            public:
                Adjacency_matrix& add_edge(const vertex_type& v1, const vertex_type& v2, typename E::value_type e) {
                    set_edge(*this, v1, v2, {e});
                    return *this;
                }
                Adjacency_matrix& add_edge(const vertex_type& v1, const vertex_type& v2) {
                    set_edge(*this, v1, v2);
                    return *this;
                }
                void remove_edge(vertex_type& v1, vertex_type& v2) {
                    set_edge(*this, v1, v2, E::empty_value());
                }
        };

        template<typename V, typename E>
            class Adjacency_matrix_base<V, E>::Vertex : public Vertex_base<V> {
                private:
                    template<bool T_is_const>
                        class Iterator;
                    template<bool T_is_const>
                        class Edges_iterator;
                    friend class Adjacency_matrix_base;
                    friend class Vector<Vertex>;

                    Adjacency_matrix_base* matrix_;

                    Vertex(const V& value, Adjacency_matrix_base* matrix) 
                        :Vertex_base<V>(value), matrix_(matrix) 
                    {}
                    Vertex() :matrix_(nullptr) {}

                    Vector<E>& edges() const {
                        return matrix_->edges_[*this];
                    }
                    template<typename It, typename VV>
                        static It create_begin_it(VV* vertex) {
                            return It(vertex, vertex->edges().begin()).move_to_non_empty();
                        }
                    template<typename It, typename VV>
                        static It create_end_it(VV* vertex) {
                            return It(vertex, vertex->edges().end());
                        }
                public:
                    size_t index() const { return this - matrix_->vertices_.cbegin(); }
                    operator size_t() const { return index(); }

                    using iterator = Iterator<false>;
                    using const_iterator = Iterator<true>;
                    using edges_iterator = Edges_iterator<false>;
                    using const_edges_iterator = Edges_iterator<true>;

                    auto cbegin() const { return create_begin_it<const_iterator>(this); }
                    auto cend() const { return create_end_it<const_iterator>(this); }
                    auto begin() { return create_begin_it<iterator>(this); }
                    auto end() { return create_end_it<iterator>(this); }

                    auto cedges_begin() const { return create_begin_it<const_edges_iterator>(this); }
                    auto cedges_end() const { return create_end_it<const_edges_iterator>(this); }
                    auto edges_begin() { return create_begin_it<edges_iterator>(this); }
                    auto edges_end() { return create_end_it<edges_iterator>(this); }
            };

        template<typename V, typename E>
            template<bool T_is_const>
            class Adjacency_matrix_base<V, E>::Vertex::Iterator {
                private:
                    using value_type = std::conditional_t<T_is_const, const Vertex, Vertex>;
                    using vertices_type = std::conditional_t<T_is_const, const Vector<Vertex>, Vector<Vertex>>;
                    friend class Vertex;
                    Iterator& move_to_non_empty() {
                        for (; it_ != edges_.end() && !*it_; ++it_);
                        return *this;
                    }
                    Iterator(value_type* vertex, const typename Vector<E>::iterator& it) 
                        :vertices_(vertex->matrix_->vertices_),
                        edges_(vertex->matrix_->edges_[vertex->index()]),
                        it_(it) {}
                    static Iterator create_begin_it(value_type* vertex) {
                        return Iterator(vertex, vertex->edges().begin()).move_to_non_empty();
                    }
                    static Iterator create_end_it(value_type* vertex) {
                        return {vertex, vertex->edges().end()};
                    }
                protected:
                    vertices_type& vertices_;
                    Vector<E>& edges_;
                    typename Vector<E>::iterator it_;
                public:
                    Iterator& operator++() {
                        ++it_;
                        move_to_non_empty();
                        return *this;
                    }
                    bool operator==(const Iterator& o) const {
                        return it_ == o.it_;
                    }
                    bool operator!=(const Iterator& o) const {
                        return !operator==(o);
                    }
                    value_type& operator*() const {
                        return vertices_[it_ - edges_.begin()];
                    }
                    value_type* operator->() const {
                        return &operator*();
                    }
            };

        template<typename V, typename E>
            template<bool T_is_const>
            class Adjacency_matrix_base<V, E>::Vertex::Edges_iterator : public Iterator<T_is_const> {
                private:
                    using Base = Iterator<T_is_const>;
                    using vertex_type = std::conditional_t<T_is_const, const Vertex, Vertex>;
                    using entry_type = Edges_iterator_entry<Vertex, E, T_is_const>;
                    friend class Vertex;

                    entry_type entry_;

                    Edges_iterator(vertex_type* vertex, const typename Vector<E>::iterator& it)
                        :Base(vertex, it), entry_(vertex)
                    {}
                    Edges_iterator& move_to_non_empty() {
                        Base::move_to_non_empty();
                        update_entry();
                        return *this;
                    }
                    void update_entry() {
                        if (Base::it_ != Base::edges_.end()) {
                            entry_.target_ = &Base::vertices_[Base::it_ - Base::edges_.begin()];
                            entry_.edge_ = &*Base::it_;
                        }
                    }
                public:
                    const entry_type& operator*() const { return entry_; }
                    const entry_type* operator->() const { return &entry_; }
                    Edges_iterator& operator++() { 
                        Base::operator++();
                        update_entry();
                        return *this;
                    }
            };

    }

    template<Graph_type graph_type, typename V, typename E = Adjacency_matrix_ns::Edge<bool>>
    using Adjacency_matrix = Adjacency_matrix_ns::Adjacency_matrix<graph_type, V, E>;

    template<Graph_type T_type, typename VV, typename EE = bool>
        Adjacency_matrix<T_type, VV, Adjacency_matrix_ns::Edge<EE>> create_adj_matrix() { return {}; }

}
