#include "adjacency_matrix.h"
#include "adjacency_lists.h"
#include "graphs.h"

#include <string>
#include <stdexcept>
#include <sstream>

template<typename P>
void print_path(const P& p) {
    Graph::print_collection(p.cbegin(), p.cend(), " - ", [](auto p) { return *p; }, std::cout);
}

template<typename G>
void test_graph(const char* label) {
    std::cout << std::endl << label << ":" << std::endl;
    G graph;
    Graph::Constructor constructor(graph);

    constructor.add_edge(1, 2)
        .add_edge(2, 3)
        .add_edge(2, 4)
        .add_edge(3, 4);
    constructor.get_or_create_vertex(5);

    auto has_simple_path = [&graph, &constructor](const auto& l1, const auto& l2) {
        std::cout << l1 << " - " << l2 << ": ";
        if (Graph::has_simple_path(graph, constructor.get_vertex(l1), constructor.get_vertex(l2)))
            std::cout << "simple path found";
        else
            std::cout << "no simple path";
        std::cout << std::endl;
    };
    has_simple_path(1, 4);
    has_simple_path(1, 5);

    std::cout << "Hamilton path: ";
    auto h_path = Graph::compose_hamilton_path(graph);
    print_path(h_path);

    std::cout << std::endl << "internal structure: " << std::endl;
    graph.print_internal(std::cout);

    std::cout << "euler tour:" << std::endl;
    graph = Graph::Samples::euler_tour_sample<G>();
    auto path = Graph::compose_euler_tour(graph, graph.vertex_at(0));
    print_path(path);
    std::cout << std::endl;

    std::cout << "graph with bridges:" << std::endl;
    graph = Graph::Samples::bridges_sample<G>();
    auto bridges = Graph::find_bridges(graph);
    auto b = bridges.begin();
    if (b != bridges.end()) {
        auto print_bridge = [](auto& b) {
            std::cout << *b->first << " - " << *b->second;
        };
        print_bridge(b);
        for (++b; b != bridges.end(); ++b) {
            std::cout << ", ";
            print_bridge(b);
        }
    }

    std::cout << std::endl << "shortest paths" << std::endl;
    graph = Graph::Samples::shortest_paths_sample<G>();
    auto matrix = find_shortest_paths(graph);

    print_path(matrix.find_path(graph.vertex_at(0), graph.vertex_at(7)));
    std::cout << std::endl;
    print_path(matrix.find_path(graph.vertex_at(1), graph.vertex_at(7)));
}

template<typename C>
void print_aligned_collection(const C& c) {
    for (auto it = c.cbegin(); it != c.cend(); ++it) {
        std::cout.width(3);
        std::cout << *it;
    }
    std::cout << std::endl;
}

template<typename G>
void test_digraph() {
    std::cout << "dfs transitive closure" << std::endl;
    auto g = Graph::Samples::digraph_sample<G>();
    auto transitive_closure = Graph::dfs_transitive_closure(g);
    transitive_closure.print_internal(std::cout);
}

int main() {

     test_graph<Graph::Adjacency_matrix<int>>("adjacency matrix");
     test_graph<Graph::Adjacency_lists<int>>("adjacency lists");

     std::cout << std::endl;
     test_digraph<Graph::Adjacency_matrix<int, Graph::Graph_type::DIGRAPH>>();
     test_digraph<Graph::Adjacency_lists<int, Graph::Graph_type::DIGRAPH>>();

     std::cout << "Warshall transitive closure" << std::endl;
     auto g = Graph::Samples::digraph_sample<Graph::Adjacency_matrix<int, Graph::Graph_type::DIGRAPH>>();
     auto transitive_closure = Graph::warshall_transitive_closure(g);
     transitive_closure.print_internal(std::cout);
     std::cout << "DAG is valid: " << Graph::is_dag(g) << std::endl;

     g = Graph::Samples::dag_sample<Graph::Adjacency_matrix<int, Graph::Graph_type::DIGRAPH>>();

     Graph::trace_dfs(g);
     std::cout << "DAG is valid: " << Graph::is_dag(g) << std::endl;

     std::cout << "topological sort: " << std::endl << Graph::topological_sort(g) << std::endl;
     std::cout << "topological sort (inverted): " << std::endl << Graph::topological_sort_inverted(g) << std::endl;
     
     Graph::topological_sort_sinks_queue(g);

     Graph::Builder<decltype(g)> b;
     for (int i = 0; i < 13; ++i)
         b.for_vertex(i);

     g = b
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

     Graph::trace_dfs(g);
     std::cout << "dfs with topological sort:" << std::endl;
     Graph::trace_dfs_topo_sorted(g);

     std::cout << "strong components: " << std::endl << calculate_strong_components(g) << std::endl;

    // auto g = Graph::Samples::euler_tour_sample<Graph::Adjacency_matrix<int>>();
    // Graph::dfs(g, [](auto& v) {
    //     std::cout << v << std::endl;
    // }, [](auto& v, auto& w) {
    //     std::cout << v << " " << w << std::endl;
    // });

}
