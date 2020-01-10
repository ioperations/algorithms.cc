#pragma once

#include "array.h"

#include <iostream>

namespace Graph {

    template<typename T>
        class Vertex_base {
            protected:
                T value_;
                size_t index_;

                Vertex_base(const T& value, size_t index) 
                    :value_(value), index_(index) 
                {}
                Vertex_base() = default;
                Vertex_base(const Vertex_base&) = delete;
                Vertex_base& operator=(const Vertex_base&) = delete;
                Vertex_base(Vertex_base&&) = default;
                Vertex_base& operator=(Vertex_base&&) = default;
            public:
                T value() const {
                    return value_;
                }
                size_t index() const {
                    return index_;
                }
                bool operator==(const Vertex_base& o) const {
                    return index_ == o.index_;
                }
                friend std::ostream& operator<<(std::ostream& stream, const Vertex_base& v) {
                    return stream << v.value_;
                }
        };

    template<typename G, typename V = typename G::Vertex>
        bool has_simple_path(const G& graph, const V& v1, const V& v2) {

            Array<bool> visited(graph.vertices_count());
            for (auto& b : visited)
                b = false;

            static struct {
                bool has_simple_path(const G& graph, const V& v1, const V& v2, Array<bool>& visited) {
                    if (v1 == v2)
                        return true;
                    visited[v1.index()] = true;
                    for (const auto v = v1.cbegin(); v != v1.cend(); ++v)
                        if (!visited[v->index()])
                            if (has_simple_path(graph, *v, v2, visited))
                                return true;
                    return false;
                }
            } helper;

            return helper.has_simple_path(graph, v1, v2, visited);
        }

}

