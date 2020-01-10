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
            public:
                Adjacency_matrix() :edges_(100) {
                    for (auto& l : edges_)
                        l = Vector<bool>(100);
                }
                Vertex& create_vertex(const T& t) {
                    vertices_.push_back(Vertex(t, vertices_.size(), this)); // todo add emplace back method
                    return vertices_[vertices_.size() - 1];
                }
                void add_edge(const Vertex& v1, const Vertex& v2) {
                    edges_[v1.index_][v2.index_] = true;
                    edges_[v2.index_][v1.index_] = true;
                }
                size_t vertices_count() const {
                    return vertices_.size();
                }
                const Vertex& vertex_at(size_t index) const {
                    return vertices_[index];
                }
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
                friend class Adjacency_matrix;
                friend class Vector<Vertex>;

                Adjacency_matrix* matrix_;

                Vertex(const T& value, size_t index, Adjacency_matrix* matrix) 
                    :Vertex_base<T>(value, index), matrix_(matrix) 
                {}
                Vertex() :matrix_(nullptr) {}
            public:
                class Iterator;
                const Iterator cbegin() const {
                    auto& e = matrix_->edges_[this->index()];
                    auto it = Iterator(*this, e.begin());
                    it.move_to_non_empty();
                    return it;
                }
                const Iterator cend() const {
                    auto& e = matrix_->edges_[this->index()];
                    return Iterator(*this, e.end());
                }
                Iterator begin() {
                    return const_cast<Iterator>(static_cast<const Iterator&>(*this).cbegin());
                }
                Iterator end() {
                    return const_cast<Iterator>(static_cast<const Iterator&>(*this).cend());
                }
        };

    template<typename T>
        class Adjacency_matrix<T>::Vertex::Iterator {
            private:
                friend class Vertex;
                Vector<Vertex>& vertices_;
                Vector<bool>& edges_;
                mutable Vector<bool>::iterator it_;
                void move_to_non_empty() const {
                    for (; it_ != edges_.end() && !*it_; ++it_);
                }
                Iterator(const Vertex& vertex, const Vector<bool>::iterator& it) 
                    :vertices_(vertex.matrix_->vertices_),
                    edges_(vertex.matrix_->edges_[vertex.index()]),
                    it_(it) {}
            public:
                const Iterator& operator++() const {
                    ++it_;
                    move_to_non_empty();
                    return *this;
                }
                Iterator& operator++() {
                    return const_cast<Iterator&>(++static_cast<const Iterator&>(*this));
                }
                bool operator!=(const Iterator& o) const {
                    return it_ != o.it_;
                }
                const Vertex* operator->() const {
                    return &operator*();
                }
                Vertex* operator->() {
                    return const_cast<Vertex*>(static_cast<const Iterator&>(*this).operator->());
                }
                const Vertex& operator*() const {
                    return vertices_[it_ - edges_.begin()];
                }
                Vertex& operator*() {
                    return const_cast<Vertex&>(*static_cast<const Iterator&>(*this));
                }
        };

}
