#pragma once

#include <iostream>

#include "vector.h"

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
            public:
                class Iterator {
                    private:
                        friend class Vertex;
                        Vertex& vertex_;
                        Vector<bool>::iterator it_;
                        Iterator& move_to_non_empty() {
                            for (; it_ != vertex_.matrix_->edges_[vertex_.index_].end() && !*it_; ++it_);
                            return *this;
                        }
                    public:
                        Iterator(Vertex& vertex, const Vector<bool>::iterator& it) 
                            :vertex_(vertex), it_(it) 
                        {}
                        Iterator& operator++() {
                            ++it_;
                            move_to_non_empty();
                            return *this;
                        }
                        bool operator!=(const Iterator& o) {
                            return it_ != o.it_;
                        }
                        Vertex* operator->() {
                            auto v_index = it_ - vertex_.matrix_->edges_[vertex_.index_].begin();
                            return &vertex_.matrix_->vertices_[v_index];
                        }
                };
                Iterator begin() {
                    auto& e = matrix_->edges_[index_];
                    return Iterator(*this, e.begin()).move_to_non_empty();
                }
                Iterator end() {
                    auto& e = matrix_->edges_[index_];
                    return Iterator(*this, e.end());
                }
                friend std::ostream& operator<<(std::ostream& stream, const Vertex& v) {
                    return stream << v.value(); 
                }
        };
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
        void print_internal(std::ostream& stream) {
            auto size = vertices_.size();
            for (size_t r = 0; r < size; ++r) {
                for (size_t c = 0; c < size; ++c) {
                    stream << edges_[r][c] << " ";
                }
                stream << std::endl;
            }
        }
        void iterate() {
            auto& v = vertices_[1];
            for (auto it = v.begin(); it != v.end(); ++it)
                std::cout << it->value() << std::endl;
        }
};

