#include "gtest/gtest.h"

#include "adjacency_matrix.h"
#include "adjacency_lists.h"
#include "graphs.h"

using namespace Graph;

template<typename P>
std::string path_to_string(const P& p) {
    std::stringstream ss;
    print_collection(p.cbegin(), p.cend(), " - ", [](auto p) -> auto& { return *p; }, ss);
    return ss.str();
}

template<typename G>
void test_graph() {
    G graph;
    Constructor constructor(graph);

    constructor.add_edge(1, 2)
        .add_edge(2, 3)
        .add_edge(2, 4)
        .add_edge(3, 4);
    constructor.get_or_create_vertex(5);

    auto has_simple_path = [&graph, &constructor](const auto& l1, const auto& l2) {
        return ::has_simple_path(graph, constructor.get_vertex(l1), constructor.get_vertex(l2));
    };
    ASSERT_TRUE(has_simple_path(1, 4));
    ASSERT_FALSE(has_simple_path(1, 5));

    graph = Samples::hamilton_path_sample<G>();
    auto h_path = compose_hamilton_path(graph, graph[0], graph[1]);
    ASSERT_EQ("0 - 6 - 4 - 5 - 3 - 2 - 1", path_to_string(h_path));

    graph = Samples::euler_tour_sample<G>();
    auto euler_tour = compose_euler_tour(graph, graph[0]);
    ASSERT_EQ("0 - 6 - 4 - 3 - 2 - 4 - 5 - 0 - 2 - 1 - 0", path_to_string(euler_tour));

    graph = Samples::bridges_sample<G>();
    auto bridges = find_bridges(graph);
    auto b = bridges.begin();
    std::stringstream ss;
    if (b != bridges.end()) {
        auto print_bridge = [&ss](auto& b) {
            ss << *b->first << " - " << *b->second;
        };
        print_bridge(b);
        for (++b; b != bridges.end(); ++b) {
            ss << ", ";
            print_bridge(b);
        }
    }
    ASSERT_EQ("6 - 7, 11 - 12, 0 - 5", ss.str());

    graph = Samples::shortest_paths_sample<G>();
    auto matrix = find_shortest_paths(graph);
    ASSERT_EQ("0 - 7 - 1", path_to_string(matrix.find_path(graph[0], graph[7])));
    ASSERT_EQ("2 - 0 - 7 - 1", path_to_string(matrix.find_path(graph[1], graph[7])));
}

TEST(Graphs_algorithms_test, find_lca) {
    test_graph<Adjacency_matrix<Graph_type::GRAPH, int>>();
    test_graph<Adjacency_lists<Graph_type::GRAPH, int>>();
}

