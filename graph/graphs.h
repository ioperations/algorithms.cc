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

    }

}
