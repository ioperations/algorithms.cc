#pragma once

#include "array.h"
#include "stack.h"

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

    template<typename G, typename V = typename G::Vertex>
        Array<const V*> compose_hamilton_path(const G& graph) {
            if (graph.vertices_count() < 1)
                return {};
            if (graph.vertices_count() < 2)
                return {&graph.vertex_at(0)};

            Array<bool> visited(graph.vertices_count());
            for (auto& b : visited)
                b = false;
            // std::cout << std::endl;
            using Stack = Stack<const V* const>;
            static struct {
                bool has_hamilton_path(const G& graph, const V& v1, const V& v2,
                                       Array<bool>& visited, size_t depth, Stack& stack) {
                    // std::cout.width((graph.vertices_count() - depth) * 2);
                    // std::cout << v1 << " " << v2 << " (" << depth << ")" << std::endl;
                    bool has;
                    if (v1 == v2)
                        has = depth == 0;
                    else {
                        has = false;
                        visited[v1.index()] = true;
                        for (auto v = v1.cbegin(); v != v1.cend() && !has; ++v)
                            has = !visited[v->index()] 
                                && has_hamilton_path(graph, *v, v2, visited, depth - 1, stack);
                        if (!has)
                            visited[v1.index()] = false;
                    }
                    if (has)
                        stack.push(&v1);
                    return has;
                }
            } helper;
            Stack stack;
            Array<const V*> path;
            if (helper.has_hamilton_path(graph, graph.vertex_at(0), graph.vertex_at(1), visited,
                                            graph.vertices_count() - 1, stack)) {
                path = Array<const V*>(graph.vertices_count());
                size_t index = 0;
                while (!stack.empty())
                    path[index++] = stack.pop();

            }
            return path;
        }

}

