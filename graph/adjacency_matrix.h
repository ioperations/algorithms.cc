#pragma once

#include <iostream>

#include "graph.h"
#include "vector.h"

namespace Graph {

    template<typename T>
        class Adjacency_matrix_base {
            public:
                using value_type = T;
                class Vertex;
            private:
                friend class Vertex;
                Vector<Vertex> vertices_;
                void update_vertices_this_link() {
                    for (auto& v : vertices_)
                        v.matrix_ = this;
                }
            protected:
                Vector<Vector<bool>> edges_;

            public:
                Adjacency_matrix_base() :edges_(100) {
                    for (auto& l : edges_) l = Vector<bool>(100);
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

                Vertex& create_vertex(const T& t) {
                    vertices_.push_back(Vertex(t, vertices_.size(), this)); // todo add emplace back method
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
                bool has_edge(const Vertex& v, const Vertex& w) {
                    return edges_[v][w];
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

    template<typename T, Graph_type graph_type = Graph_type::GRAPH>
        class Adjacency_matrix : public Adjacency_matrix_base<T> {
            public:
                using Vertex = typename Adjacency_matrix_base<T>::Vertex;
            private:
                template<typename TT, Graph_type gt = Graph_type::GRAPH>
                    struct Edges_handler {
                        static void set_edge(Vector<Vector<bool>>& edges, const Vertex& v1, const Vertex& v2,
                                             bool is_set) {
                            edges[v1][v2] = is_set;
                            edges[v2][v1] = is_set;
                        }
                    };
                template<typename TT>
                    struct Edges_handler<TT, Graph_type::DIGRAPH> {
                        static void set_edge(Vector<Vector<bool>>& edges, const Vertex& v1, const Vertex& v2,
                                             bool is_set) {
                            edges[v1][v2] = is_set;
                        }
                    };
            public:
                Adjacency_matrix& add_edge(const Vertex& v1, const Vertex& v2) {
                    Edges_handler<T, graph_type>::set_edge(Adjacency_matrix_base<T>::edges_, v1, v2, true);
                    return *this;
                }
                void remove_edge(Vertex& v1, Vertex& v2) {
                    Edges_handler<T, graph_type>::set_edge(Adjacency_matrix_base<T>::edges_, v1, v2, false);
                }
        };


    template<typename T>
        class Adjacency_matrix_base<T>::Vertex : public Vertex_base<T> {
            private:
                template<bool T_is_const>
                    class Iterator;
                friend class Adjacency_matrix_base;
                friend class Vector<Vertex>;

                Adjacency_matrix_base* matrix_;

                Vertex(const T& value, size_t index, Adjacency_matrix_base* matrix) 
                    :Vertex_base<T>(value, index), matrix_(matrix) 
                {}
                Vertex() :matrix_(nullptr) {}

                Vector<bool>& edges() const {
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
        };

    template<typename T>
        template<bool T_is_const>
        class Adjacency_matrix_base<T>::Vertex::Iterator {
            private:
                using value_type = std::conditional_t<T_is_const, const Vertex, Vertex>;
                friend class Vertex;
                Vector<Vertex>& vertices_;
                Vector<bool>& edges_;
                Vector<bool>::iterator it_;
                Iterator& move_to_non_empty() {
                    for (; it_ != edges_.end() && !*it_; ++it_);
                    return *this;
                }
                Iterator(const Vertex& vertex, const Vector<bool>::iterator& it) 
                    :vertices_(vertex.matrix_->vertices_),
                    edges_(vertex.matrix_->edges_[vertex.index()]),
                    it_(it) {}
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
                value_type* operator->() const {
                    return &operator*();
                }
                value_type& operator*() const {
                    return vertices_[it_ - edges_.begin()];
                }
        };

}
