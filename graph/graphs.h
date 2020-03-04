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

        template<typename G>
            auto dag_sample() {
                G g;
                auto& v0 = g.create_vertex(0);
                auto& v1 = g.create_vertex(1);
                auto& v2 = g.create_vertex(2);
                auto& v3 = g.create_vertex(3);
                auto& v4 = g.create_vertex(4);
                auto& v5 = g.create_vertex(5);
                auto& v6 = g.create_vertex(6);
                auto& v7 = g.create_vertex(7);
                auto& v8 = g.create_vertex(8);
                auto& v9 = g.create_vertex(9);
                auto& v10 = g.create_vertex(10);
                auto& v11 = g.create_vertex(11);
                auto& v12 = g.create_vertex(12);
                return g
                    .add_edge(v0, v1)
                    .add_edge(v0, v2)
                    .add_edge(v0, v3)
                    .add_edge(v0, v5)
                    .add_edge(v0, v6)
                    .add_edge(v2, v3)
                    .add_edge(v3, v4)
                    .add_edge(v3, v5)
                    .add_edge(v4, v9)
                    .add_edge(v6, v4)
                    .add_edge(v6, v9)
                    .add_edge(v7, v6)
                    .add_edge(v8, v7)
                    .add_edge(v9, v10)
                    .add_edge(v9, v11)
                    .add_edge(v9, v12)
                    .add_edge(v11, v12);
            }

        template<typename G>
            G strong_components_sample() {
                Graph::Builder<G> b;
                for (int i = 0; i < 13; ++i)
                    b.for_vertex(i);
                return b
                    .for_vertex(0).add_edges(1, 5, 6)
                    .for_vertex(2).add_edges(0, 3)
                    .for_vertex(3).add_edges(2, 5)
                    .for_vertex(4).add_edges(2, 3, 11)
                    .for_vertex(5).add_edges(4)
                    .for_vertex(6).add_edges(4, 9)
                    .for_vertex(7).add_edges(6, 8)
                    .for_vertex(8).add_edges(7, 9)
                    .for_vertex(9).add_edges(10, 11)
                    .for_vertex(10).add_edges(12)
                    .for_vertex(11).add_edges(12)
                    .for_vertex(12).add_edges(9)
                    .build();
            }

        template<typename G>
            G weighted_graph_sample() {
                Graph::Builder<G> b;
                for (int i = 0; i < 8; ++i)
                    b.for_vertex(i);
                return b
                    .for_vertex(0).add_edge(1, .32).add_edge(2, .29).add_edge(5, .60).add_edge(6, .51).add_edge(7, .31)
                    .for_vertex(1).add_edge(7, .21)
                    .for_vertex(3).add_edge(5, .18).add_edge(4, .34)
                    .for_vertex(4).add_edge(5, .40).add_edge(6, .51).add_edge(7, .46)
                    .for_vertex(5)
                    .for_vertex(6).add_edge(7, .25)
                    .for_vertex(7)
                    .build();
            }

        template<typename G>
            G spt_sample() {
                Graph::Builder<G> b;
                for (int i = 0; i < 6; ++i)
                    b.for_vertex(i);
                return b
                    .for_vertex(0).add_edge(1, .41).add_edge(3, .45).add_edge(5, .29)
                    .for_vertex(1).add_edge(2, .51).add_edge(4, .21).add_edge(5, .29)
                    .for_vertex(2).add_edge(3, .5).add_edge(4, .32)
                    .for_vertex(4).add_edge(5, .21)
                    .build();
            }

        template<typename G>
            G weighted_dag_sample() {
                Graph::Builder<G> b;
                for (int i = 0; i < 9; ++i)
                    b.for_vertex(i);
                return b
                    .for_vertex(0).add_edge(1, .41).add_edge(7, .41).add_edge(9, .41)
                    .for_vertex(1).add_edge(2, .51)
                    .for_vertex(6).add_edge(3, .21).add_edge(8, .21)
                    .for_vertex(7).add_edge(3, .32).add_edge(8, .32)
                    .for_vertex(8).add_edge(2, .32)
                    .for_vertex(9).add_edge(4, .29).add_edge(6, .29)
                    .build();
            }

        template<typename G>
            G flow_sample() {
                Builder<G> b;
                for (int i = 0; i < 6; ++i)
                    b.for_vertex(i);
                return b
                    .for_vertex(0).add_edge(1, {2, 0}).add_edge(2, {3, 0})
                    .for_vertex(1).add_edge(3, {3, 0}).add_edge(4, {1, 0})
                    .for_vertex(2).add_edge(3, {1, 0}).add_edge(4, {1, 0})
                    .for_vertex(3).add_edge(5, {2, 0})
                    .for_vertex(4).add_edge(5, {3, 0})
                    .build();
            }
    }

}
