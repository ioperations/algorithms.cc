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

                Vertex(const T& value, size_t index) :value_(value), index_(index) {}
                Vertex() {}

                Vertex(const Vertex&) = delete;
                Vertex& operator=(const Vertex&) = delete;
                Vertex(Vertex&&) = default;
                Vertex& operator=(Vertex&&) = default;

                T value() const {
                    return value_;
                }
            public:
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
            vertices_.push_back(Vertex(t, vertices_.size())); // todo add emplace back method
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
};

