#pragma once

#include "array.h"

#include <iostream>

namespace Graph {

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

