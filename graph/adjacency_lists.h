#pragma once

#include <iostream>

#include "vector"
#include "forward_list.h"

template<typename T>
class Adjacency_lists {
    public:
        class Vertex {
            private:
                friend class Adjacency_lists;
                friend class Vector<Vertex>;

                T value_;
                size_t index_;
                Forward_list<size_t> links_;

                Vertex(T value, size_t index) :value_(value), index_(index) {}
                Vertex() {}

                Vertex(const Vertex&) = delete;
                Vertex& operator=(const Vertex&) = delete;
                Vertex(Vertex&&) = default;
                Vertex& operator=(Vertex&&) = default;
            public:
                friend std::ostream& operator<<(std::ostream& stream, const Vertex& v) {
                    return stream;
                }
        };
    private:
        Vector<Vertex> vertices_;
    public:
        Vertex& create_vertex(const T& t) {
            vertices_.push_back(Vertex(t, vertices_.size()));
            return vertices_[vertices_.size() - 1];
        }
        void add_edge(const Vertex& v1, const Vertex& v2) {
            // todo
        }
};


