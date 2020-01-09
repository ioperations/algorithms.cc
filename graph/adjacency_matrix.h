#pragma once

#include <iostream>

#include "vector.h"
#include "array.h"

template<typename T>
class Adjacency_matrix {
    public:
        class Vertex {
            private:
                friend class Adjacency_matrix;
                friend class Vector<Vertex>;

                T value_;
                size_t index_;
                Adjacency_matrix* matrix_;

                Vertex(const T& value, size_t index, Adjacency_matrix* matrix) 
                    :value_(value), index_(index), matrix_(matrix) 
                {}
                Vertex() :matrix_(nullptr) {}

                Vertex(const Vertex&) = delete;
                Vertex& operator=(const Vertex&) = delete;
                Vertex(Vertex&&) = default;
                Vertex& operator=(Vertex&&) = default;

                T value() const {
                    return value_;
                }
                size_t index() const {
                    return index_;
                }
            public:
                class Iterator {
                    private:
                        friend class Vertex;
                        Vector<Vertex>& vertices_;
                        Vector<bool>& edges_;
                        mutable Vector<bool>::iterator it_;
                        void move_to_non_empty() const {
                            for (; it_ != edges_.end() && !*it_; ++it_);
                        }
                    public:
                        Iterator(const Vertex& vertex, const Vector<bool>::iterator& it) 
                            :vertices_(vertex.matrix_->vertices_),
                            edges_(vertex.matrix_->edges_[vertex.index_]),
                            it_(it) {}
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
                const Iterator cbegin() const {
                    auto& e = matrix_->edges_[index_];
                    auto it = Iterator(*this, e.begin());
                    it.move_to_non_empty();
                    return it;
                }
                const Iterator cend() const {
                    auto& e = matrix_->edges_[index_];
                    return Iterator(*this, e.end());
                }
                Iterator begin() {
                    return const_cast<Iterator>(static_cast<const Iterator&>(*this).cbegin());
                }
                Iterator end() {
                    return const_cast<Iterator>(static_cast<const Iterator&>(*this).cend());
                }
                bool operator==(const Vertex& o) const {
                    return index_ == o.index_;
                }
                friend std::ostream& operator<<(std::ostream& stream, const Vertex& v) {
                    return stream << v.value(); 
                }
        };
    private:
        friend class Vertex;
        Vector<Vertex> vertices_;
        Vector<Vector<bool>> edges_;

        bool has_simple_path(const Vertex& v1, const Vertex& v2, Array<bool>& visited) {
            if (v1 == v2)
                return true;
            visited[v1.index_] = true;
            for (const auto v = v1.cbegin(); v != v1.cend(); ++v)
                if (!visited[v->index()])
                    if (has_simple_path(*v, v2, visited))
                        return true;
            return false;
        }
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
        void print_internal(std::ostream& stream) {
            auto size = vertices_.size();
            for (size_t r = 0; r < size; ++r) {
                for (size_t c = 0; c < size; ++c) {
                    stream << edges_[r][c] << " ";
                }
                stream << std::endl;
            }
        }
        bool has_simple_path(const Vertex& v1, const Vertex& v2) {
            Array<bool> visited(vertices_.size());
            return has_simple_path(v1, v2, visited);
        }
};

