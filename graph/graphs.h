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

    }

}
