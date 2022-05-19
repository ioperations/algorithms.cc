#include "adjacency_lists.h"
#include "adjacency_matrix.h"
#include "graphs.h"
#include "gtest/gtest.h"
#include "test_utils.h"

using namespace Graph;

template <typename P>
std::string path_to_string(const P& p) {
    std::stringstream ss;
    print_collection(
        p.cbegin(), p.cend(), " - ", [](auto p) -> auto& { return *p; }, ss);
    return ss.str();
}

template <typename G>
void test_graph() {
    G graph;
    Constructor constructor(graph);

    constructor.add_edge(1, 2).add_edge(2, 3).add_edge(2, 4).add_edge(3, 4);
    constructor.get_or_create_vertex(5);

    auto has_simple_path = [&graph, &constructor](const auto& l1,
                                                  const auto& l2) {
        return ::has_simple_path(graph, constructor.get_vertex(l1),
                                 constructor.get_vertex(l2));
    };
    ASSERT_TRUE(has_simple_path(1, 4));
    ASSERT_FALSE(has_simple_path(1, 5));

    graph = Samples::hamilton_path_sample<G>();
    auto h_path = compose_hamilton_path(graph, graph[0], graph[1]);
    ASSERT_EQ("0 - 6 - 4 - 5 - 3 - 2 - 1", path_to_string(h_path));

    graph = Samples::euler_tour_sample<G>();
    auto euler_tour = compose_euler_tour(graph, graph[0]);
    ASSERT_EQ("0 - 6 - 4 - 3 - 2 - 4 - 5 - 0 - 2 - 1 - 0",
              path_to_string(euler_tour));

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
    ASSERT_EQ("0 - 7 - 1",
              path_to_string(matrix.find_path(graph[0], graph[7])));
    ASSERT_EQ("2 - 0 - 7 - 1",
              path_to_string(matrix.find_path(graph[1], graph[7])));
}

TEST(Graphs_algorithms_test, graph) {
    test_graph<AdjacencyMatrix<GraphType::GRAPH, int>>();
    test_graph<AdjacencyLists<GraphType::GRAPH, int>>();
}

template <typename G>
std::string graph_to_str_matrix(const G& g) {
    Array<Array<bool>> matrix(g.vertices_count(),
                              Array<bool>(g.vertices_count(), false));
    for (auto v = g.cbegin(); v != g.cend(); ++v)
        for (auto w = v->cbegin(); w != v->cend(); ++w) matrix[*v][*w] = true;
    std::stringstream ss;
    for (auto& r : matrix) {
        for (bool b : r) ss << b << " ";
        ss << std::endl;
    }
    return ss.str();
}

template <typename G>
void test_digraph() {
    auto g = Samples::digraph_sample<G>();

    std::stringstream ss;
    trace_dfs(g, reset_with_new_line(ss));
    ASSERT_EQ(R"(
0
 5 (down)
  4 (down)
   5 (back)
 2 (down)
  1 (down)
   0 (back)
3
 4 (cross)
 2 (cross)
)",
              ss.str());

    auto transitive_closure = dfs_transitive_closure(g);
    ASSERT_EQ(R"(
1 1 1 0 1 1 
1 1 1 0 1 1 
1 1 1 0 1 1 
1 1 1 1 1 1 
0 0 0 0 1 1 
0 0 0 0 1 1 
)",
              std::string("\n") + graph_to_str_matrix(transitive_closure));
    ASSERT_FALSE(is_dag(g));

    ASSERT_EQ("[3, 2, 1, 0, 5, 4]", stringify(topological_sort_rearrange(g)));
    ASSERT_EQ("[3, 2, 1, 0, 5, 4]", stringify(topological_sort_relabel(g)));

    g = Samples::dag_sample<G>();
    ASSERT_TRUE(is_dag(g));

    ASSERT_EQ("[0, 8, 1, 2, 7, 3, 6, 5, 4, 9, 10, 11, 12]",
              stringify(topological_sort_sinks_queue(g)));

    g = Samples::strong_components_sample<decltype(g)>();

    trace_dfs(g, reset_with_new_line(ss));
    ASSERT_EQ(R"(
0
 6 (down)
  9 (cross)
  4 (cross)
 5 (down)
  4 (down)
   11 (down)
    12 (down)
     9 (down)
      11 (back)
      10 (down)
       12 (back)
   3 (down)
   2 (down)
    3 (down)
     5 (back)
     2 (back)
    0 (back)
 1 (down)
7
 8 (down)
  9 (cross)
  7 (back)
 6 (cross)
)",
              ss.str());

    trace_dfs_topo_sorted(g, reset_with_new_line(ss));
    ASSERT_EQ(R"(
9
 11 (down)
  12 (cross)
 10 (down)
  12 (down)
   9 (back)
0
 6 (down)
  9 (cross)
  4 (cross)
 5 (down)
  4 (down)
   11 (cross)
   3 (down)
   2 (down)
    3 (down)
     5 (back)
     2 (back)
    0 (back)
 1 (cross)
7
 8 (down)
  9 (cross)
  7 (back)
 6 (cross)
)",
              ss.str());

    ASSERT_EQ("[2, 1, 2, 2, 2, 2, 2, 3, 3, 0, 0, 0, 0]",
              stringify(strong_components_kosaraju(g)));
    ASSERT_EQ("[2, 1, 2, 2, 2, 2, 2, 3, 3, 0, 0, 0, 0]",
              stringify(strong_components_tarjan(g)));
}

TEST(Graphs_algorithms_test, digraph) {
    test_digraph<AdjacencyMatrix<GraphType::DIGRAPH, int>>();
    test_digraph<AdjacencyLists<GraphType::DIGRAPH, int>>();

    auto g =
        Samples::digraph_sample<AdjacencyMatrix<GraphType::DIGRAPH, int>>();
    auto transitive_closure = warshall_transitive_closure(g);
    std::stringstream ss;
    print_representation(transitive_closure, reset_with_new_line(ss));

    ASSERT_EQ(R"(
1 1 1 0 1 1 
1 1 1 0 1 1 
1 1 1 0 1 1 
1 1 1 1 1 1 
0 0 0 0 1 1 
0 0 0 0 1 1 
)",
              ss.str());
}

template <typename G>
void test_weighted_graph() {
    std::stringstream ss;
    auto g = Samples::weighted_graph_sample<G>();
    trace_dfs(pq_mst(g), reset_with_new_line(ss));
    ASSERT_EQ(R"(
0
0
 7 (0.31) (down)
  6 (0.25) (down)
  4 (0.46) (down)
   3 (0.34) (down)
    5 (0.18) (down)
  1 (0.21) (down)
 2 (0.29) (down)
  0 (0.29) (back)
)",
              ss.str());

    g = Samples::spt_sample<G>();
    Spt spt(g, g[0], g.vertices_count());
    trace_dfs(compose_path_tree(g, spt.m_spt.cbegin() + 1, spt.m_spt.cend()),
              reset_with_new_line(ss));
    ASSERT_EQ(R"(
0
0
 5 (0.29) (down)
  4 (0.21) (down)
   2 (0.32) (down)
 3 (0.45) (down)
 1 (0.41) (down)
)",
              ss.str());

    FullSpts full_spts(g, 1);
    auto diameter = full_spts.diameter();

    reset(ss);
    ss << *diameter.first;
    for (auto v = diameter.first; v != diameter.second;) {
        v = full_spts.path(*v, *diameter.second).m_source;
        ss << " - " << *v;
    }
    ASSERT_EQ("1 - 0 - 3", ss.str());
    ASSERT_EQ(.86, full_spts.distance(*diameter.first, *diameter.second));
}

TEST(Graphs_algorithms_test, weighted_graph) {
    test_weighted_graph<AdjacencyMatrix<GraphType::GRAPH, int, double>>();
    test_weighted_graph<AdjacencyLists<GraphType::GRAPH, int, double>>();

    Builder<AdjacencyLists<GraphType::DIGRAPH, int, double>> b;
    for (int i = 0; i < 6; ++i) b.for_vertex(i);
    auto g = b.for_vertex(0)
                 .add_edge(1, .41)
                 .add_edge(5, .29)
                 .for_vertex(1)
                 .add_edge(2, .51)
                 .add_edge(4, .28)
                 // .for_vertex(1).add_edge(2, .51).add_edge(4, .32)
                 .for_vertex(2)
                 .add_edge(3, .50)
                 .for_vertex(3)
                 .add_edge(0, .45)
                 .add_edge(5, -.38)
                 .for_vertex(4)
                 .add_edge(3, .36)
                 .for_vertex(5)
                 .add_edge(1, -.29)
                 .add_edge(4, .21)
                 .build();

    std::stringstream ss;
    auto n_cycle = find_negative_cycle(g, g[0], 200);
    if (!n_cycle.empty()) {
        auto v = n_cycle.cbegin();
        auto first = v;
        ss << *v;
        do {
            ++v;
            ss << "-" << *v;
        } while (v != first);
    }
    ASSERT_EQ("1-4-3-5-1", ss.str());
}

template <typename G>
void test_weighted_dag() {
    auto g = Samples::weighted_dag_sample<G>();
    validate_dag(g);

    DagLpt dag_lpt(g);
    auto lpt =
        compose_path_tree(g, dag_lpt.m_lpt.cbegin(), dag_lpt.m_lpt.cend());
    std::stringstream ss;
    trace_dfs(lpt, reset_with_new_line(ss));
    ASSERT_EQ(R"(
0
 9 (0.41) (down)
  6 (0.29) (down)
   8 (0.21) (down)
    2 (0.32) (down)
   3 (0.21) (down)
  4 (0.29) (down)
 7 (0.41) (down)
 1 (0.41) (down)
5
)",
              ss.str());
    DagFullSpts dd(g, g.vertices_count());
    ASSERT_EQ("[10, 0.41, 0.92, 0.73, 0.7, 10, 0.7, 0.41, 0.73, 0.41]",
              stringify(dd.m_distances[0]));
}

TEST(Graphs_algorithms_test, weighted_dag) {
    test_weighted_dag<AdjacencyMatrix<GraphType::DIGRAPH, int, double>>();
    test_weighted_dag<AdjacencyLists<GraphType::DIGRAPH, int, double>>();
}
