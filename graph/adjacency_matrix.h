#pragma once

#include <iostream>

#include "graph.h"
#include "vector.h"

namespace Graph {

    // todo add namespace

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
                operator bool() const { return weight_ != empty_value(); }
                void set(T weight) { weight_ = weight; }
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
                    vertices_.push_back(vertex_type(t, vertices_.size(), this)); // todo add emplace back method
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
                typename E::value_type get_edge_weight(const vertex_type& v, const vertex_type& w) {
                    return edges_[v][w].weight();
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

    template<Graph_type graph_type, typename T, typename E = Edge<bool>>
        class Adjacency_matrix : public Adjacency_matrix_base<T, E> {
            public:
                using vertex_type = typename Adjacency_matrix_base<T, E>::vertex_type;
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

    template<typename T, typename E>
        class Adjacency_matrix_base<T, E>::Vertex : public Vertex_base<T> {
            private:
                template<bool T_is_const>
                    class Iterator;
                template<bool T_is_const>
                    class Edge_iterator;
                friend class Adjacency_matrix_base;
                friend class Vector<Vertex>;

                Adjacency_matrix_base* matrix_;

                Vertex(const T& value, size_t index, Adjacency_matrix_base* matrix) 
                    :Vertex_base<T>(value, index), matrix_(matrix) 
                {}
                Vertex() :matrix_(nullptr) {}

                Vector<E>& edges() const {
                    return matrix_->edges_[*this];
                }
            public:
                using iterator = Iterator<false>;
                using const_iterator = Iterator<true>;
                const_iterator cbegin() const {
                    return const_iterator(*this, edges().begin()).move_to_non_empty();
                }
                const_iterator cend() const {
                    return {*this, edges().end()};
                }
                iterator begin() {
                    return iterator(*this, edges().begin()).move_to_non_empty();
                }
                iterator end() {
                    return {*this, edges().end()};
                }

                Edge_iterator<false> edges_begin() {
                    return Edge_iterator<false>(*this, edges().begin()).move_to_non_empty();
                }
                Edge_iterator<false> edges_end() {
                    return {*this, edges().end()};
                }
        };

    template<typename T, typename E>
        template<bool T_is_const>
        class Adjacency_matrix_base<T, E>::Vertex::Iterator {
            private:
                using value_type = std::conditional_t<T_is_const, const Vertex, Vertex>;
                friend class Vertex;
                Iterator& move_to_non_empty() {
                    for (; it_ != edges_.end() && !*it_; ++it_);
                    return *this;
                }
                Iterator(const Vertex& vertex, const typename Vector<E>::iterator& it) 
                    :vertices_(vertex.matrix_->vertices_),
                    edges_(vertex.matrix_->edges_[vertex.index()]),
                    it_(it) {}
            protected:
                Vector<Vertex>& vertices_;
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

    template<typename T, typename E>
        template<bool T_is_const>
        class Adjacency_matrix_base<T, E>::Vertex::Edge_iterator : public Iterator<T_is_const> {
            private:
                using Base = Iterator<T_is_const>;
                friend class Vertex;
                class Entry;

                Entry entry_;

                Edge_iterator(const Vertex& vertex, const typename Vector<E>::iterator& it)
                    :Base(vertex, it)
                {} // todo keep constructor in private block? is it not enough to declare inner class private?
                Edge_iterator& move_to_non_empty() {
                    Base::move_to_non_empty();
                    update_entry();
                    return *this;
                }
                void update_entry() {
                    if (Base::it_ != Base::edges_.end())
                        entry_ = {&Base::vertices_[Base::it_ - Base::edges_.begin()], &*Base::it_}; // todo end check
                }
            public:
                const Entry& operator*() const { return entry_; }
                const Entry* operator->() const { return &entry_; }
                Edge_iterator& operator++() { 
                    Base::operator++();
                    update_entry();
                    return *this;
                }
        };

    // todo rename T to V?
    template<typename T, typename E>
        template<bool T_is_const>
        class Adjacency_matrix_base<T, E>::Vertex::Edge_iterator<T_is_const>::Entry {
            public: // todo const and non-const versions
                Vertex* target_;
                E* edge_;
                // todo private constructor?
                Entry() = default;
                Entry(Vertex* target, E* edge) :target_(target), edge_(edge) {}
                const Vertex& target() const { return *target_; }
                const E& edge() const { return *edge_; }
        };

}
