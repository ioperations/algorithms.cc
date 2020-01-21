#pragma once

#include "graph.h"

namespace Graph {

    namespace Samples {

        template<typename C>
            auto construct_euler_tour_sample(C& constructor) {
                constructor
                    .add_edge(0, 1)
                    .add_edge(0, 2)
                    .add_edge(0, 5)
                    .add_edge(0, 6)
                    .add_edge(1, 2)
                    .add_edge(2, 3)
                    .add_edge(2, 4)
                    .add_edge(3, 4)
                    .add_edge(4, 5)
                    .add_edge(4, 6);
            }

        template<typename G>
            auto euler_tour_sample() {
                G graph;
                auto constructor = Graph::Constructor(graph);
                construct_euler_tour_sample(constructor);
                return graph;
            }

        template<typename G>
            auto hamilton_path_sample() {
                G graph;
                auto constructor = Graph::Constructor(graph);
                construct_euler_tour_sample(constructor);
                constructor
                    .add_edge(1, 3)
                    .add_edge(3, 5);
                return graph;
            }

        template<typename G>
            auto bridges_sample() {
                G graph;
                Graph::Constructor(graph)
                    .add_edge(0, 1)
                    .add_edge(0, 6)
                    .add_edge(1, 2)
                    .add_edge(2, 6)
                    .add_edge(6, 7)
                    .add_edge(7, 8)
                    .add_edge(7, 10)
                    .add_edge(8, 10)
                    .add_edge(0, 5)
                    .add_edge(5, 3)
                    .add_edge(5, 4)
                    .add_edge(3, 4)
                    .add_edge(4, 9)
                    .add_edge(4, 11)
                    .add_edge(9, 11)
                    .add_edge(11, 12);
                return graph;
            }

        template<typename G>
            auto shortest_paths_sample() {
                G graph;
                Graph::Constructor(graph)
                    .add_edge(0, 2)
                    .add_edge(0, 5)
                    .add_edge(0, 7)
                    .add_edge(2, 6)
                    .add_edge(5, 3)
                    .add_edge(5, 4)
                    .add_edge(3, 4)
                    .add_edge(1, 7)
                    .add_edge(4, 6)
                    .add_edge(4, 7);
                return graph;
            }

        template<typename G>
            auto digraph_sample() {
                G g;
                auto& v0 = g.create_vertex(0);
                auto& v1 = g.create_vertex(1);
                auto& v2 = g.create_vertex(2);
                auto& v3 = g.create_vertex(3);
                auto& v4 = g.create_vertex(4);
                auto& v5 = g.create_vertex(5);
                g
                    .add_edge(v0, v2)
                    .add_edge(v0, v5)
                    .add_edge(v5, v4)
                    .add_edge(v4, v5)
                    .add_edge(v3, v4)
                    .add_edge(v3, v2)
                    .add_edge(v2, v1)
                    .add_edge(v1, v0);
                return g;
            }

    }

}
