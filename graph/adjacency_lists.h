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

                void add_link(const Vertex& v) {
                    bool found = false;
                    for (auto link = links_.begin(); link != links_.end() && !found; ++link)
                        found = *link == v.index_;
                    if (!found)
                        links_.push_back(v.index_);
                }
            public:
                friend std::ostream& operator<<(std::ostream& stream, const Vertex& v) {
                    return stream << v.value_;
                }
        };
    private:
        Vector<Vertex> vertices_;
    public:
        Vertex& create_vertex(const T& t) {
            vertices_.push_back(Vertex(t, vertices_.size()));
            return vertices_[vertices_.size() - 1];
        }
        void add_edge(Vertex& v1, Vertex& v2) {
            v1.add_link(v2);
            v2.add_link(v1);
        }
        void print_internal(std::ostream& stream) {
            for (auto& v : vertices_) {
                stream << v.index_ << ": ";
                for (auto& i : v.links_)
                    stream << i << " ";
                stream << std::endl;
            }
        }
        bool has_simple_path(const Vertex& v1, const Vertex& v2) {
            return false;
        }
};

