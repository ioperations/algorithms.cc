#pragma once

#include <iostream>

#include "graph.h"
#include "vector.h"

namespace Graph {

    template<typename T>
        class Adjacency_matrix {
            public:
                using value_type = T;
                class Vertex;
            private:
                friend class Vertex;
                Vector<Vertex> vertices_;
                Vector<Vector<bool>> edges_;
                void set_edge(const Vertex& v1, const Vertex& v2, bool is_set) {
                    edges_[v1.index_][v2.index_] = is_set;
                    edges_[v2.index_][v1.index_] = is_set;
                }
                void update_vertices_this_link() {
                    for (auto& v : vertices_)
                        v.matrix_ = this;
                }
            public:
                Adjacency_matrix() :edges_(100) {
                    for (auto& l : edges_) l = Vector<bool>(100);
                }

                Adjacency_matrix(const Adjacency_matrix& o) :vertices_(o.vertices_), edges_(o.edges_) {
                    update_vertices_this_link();
                }
                Adjacency_matrix& operator=(const Adjacency_matrix& o) {
                    auto copy = o;
                    std::swap(*this, copy);
                    return *this;
                }
                Adjacency_matrix(Adjacency_matrix&& o) 
                    :vertices_(std::move(o.vertices_)), edges_(std::move(o.edges_)) {
                        update_vertices_this_link();
                    }
                Adjacency_matrix& operator=(Adjacency_matrix&& o) {
                    std::swap(vertices_, o.vertices_);
                    std::swap(edges_, o.edges_);
                    update_vertices_this_link();
                    return *this;
                }

                Vertex& create_vertex(const T& t) {
                    vertices_.push_back(Vertex(t, vertices_.size(), this)); // todo add emplace back method
                    return vertices_[vertices_.size() - 1];
                }
                void add_edge(const Vertex& v1, const Vertex& v2) {
                    set_edge(v1, v2, true);
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
                void remove_edge(Vertex& v1, Vertex& v2) {
                    set_edge(v1, v2, false);
                }

                auto cbegin() const { return vertices_.cbegin(); }
                auto cend() const { return vertices_.cend(); }

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

    template<typename T>
        class Adjacency_matrix<T>::Vertex : public Vertex_base<T> {
            private:
                template<bool T_is_const>
                    class Iterator;
                friend class Adjacency_matrix;
                friend class Vector<Vertex>;

                Adjacency_matrix* matrix_;

                Vertex(const T& value, size_t index, Adjacency_matrix* matrix) 
                    :Vertex_base<T>(value, index), matrix_(matrix) 
                {}
                Vertex() :matrix_(nullptr) {}

                Vector<bool>& edges() const {
                    return matrix_->edges_[this->index()];
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
        class Adjacency_matrix<T>::Vertex::Iterator {
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
