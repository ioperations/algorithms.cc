#include "adjacency_matrix.h"
#include "adjacency_lists.h"
#include "graphs.h"

#include <string>
#include <stdexcept>
#include <sstream>

using namespace Graph;

template<typename P>
void print_path(const P& p) {
    print_collection(p.cbegin(), p.cend(), " - ", [](auto p) { return *p; }, std::cout);
}

template<typename G>
void test_graph(const char* label) {
    std::cout << std::endl << label << ":" << std::endl;
    G graph;
    Constructor constructor(graph);

    constructor.add_edge(1, 2)
        .add_edge(2, 3)
        .add_edge(2, 4)
        .add_edge(3, 4);
    constructor.get_or_create_vertex(5);

    auto has_simple_path = [&graph, &constructor](const auto& l1, const auto& l2) {
        std::cout << l1 << " - " << l2 << ": ";
        if (::has_simple_path(graph, constructor.get_vertex(l1), constructor.get_vertex(l2)))
            std::cout << "simple path found";
        else
            std::cout << "no simple path";
        std::cout << std::endl;
    };
    has_simple_path(1, 4);
    has_simple_path(1, 5);

    std::cout << "Hamilton path: ";
    auto h_path = compose_hamilton_path(graph);
    print_path(h_path);

    std::cout << std::endl << "internal structure: " << std::endl;
    graph.print_internal(std::cout);

    std::cout << "euler tour:" << std::endl;
    graph = Samples::euler_tour_sample<G>();
    auto path = compose_euler_tour(graph, graph.vertex_at(0));
    print_path(path);
    std::cout << std::endl;

    std::cout << "graph with bridges:" << std::endl;
    graph = Samples::bridges_sample<G>();
    auto bridges = find_bridges(graph);
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
    graph = Samples::shortest_paths_sample<G>();
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
    auto g = Samples::digraph_sample<G>();
    auto transitive_closure = dfs_transitive_closure(g);
    transitive_closure.print_internal(std::cout);

    g = Samples::digraph_sample<G>();

    trace_dfs(g);
    std::cout << "DAG is valid: " << is_dag(g) << std::endl;

    std::cout << "topological sort (rearrange): " << std::endl << topological_sort_rearrange(g) << std::endl;
    std::cout << "topological sort (relabel): " << std::endl << topological_sort_relabel(g) << std::endl;

    topological_sort_sinks_queue(g);

    g = Samples::strong_components_sample<decltype(g)>();

    trace_dfs(g);
    std::cout << "dfs with topological sort:" << std::endl;
    trace_dfs_topo_sorted(g);

    std::cout << "strong components (Kosaraju): " << std::endl << strong_components_kosaraju(g) << std::endl;
    std::cout << "strong components (Tarjan)" << std::endl << strong_components_tarjan(g) << std::endl;
}

int main() {
    test_graph<Adjacency_matrix<Graph_type::GRAPH, int>>("adjacency matrix");
    test_graph<Adjacency_lists<Graph_type::GRAPH, int>>("adjacency lists");

    std::cout << "Warshall transitive closure" << std::endl;
    auto g = Samples::digraph_sample<Adjacency_matrix<Graph_type::DIGRAPH, int>>();
    auto transitive_closure = warshall_transitive_closure(g);
    transitive_closure.print_internal(std::cout);

    test_digraph<Adjacency_matrix<Graph_type::DIGRAPH, int>>();
    test_digraph<Adjacency_lists<Graph_type::DIGRAPH, int>>();

    // auto g = Samples::euler_tour_sample<Adjacency_matrix<int>>();
    // dfs(g, [](auto& v) {
    //     std::cout << v << std::endl;
    // }, [](auto& v, auto& w) {
    //     std::cout << v << " " << w << std::endl;
    // });


    {
        auto gw = create_adj_matrix<Graph_type::DIGRAPH, int, int>();
        auto& v1 = gw.create_vertex(1);
        auto& v2 = gw.create_vertex(2);
        auto& v3 = gw.create_vertex(3);
        gw.add_edge(v1, v2, 4);
        gw.add_edge(v1, v3, 8);

        auto& v = *gw.begin();
        v.set_value(22);

        // todo return iterator
        std::cout << gw.get_edge_weight(v1, v2) << std::endl;

        v1.edges_begin()->target().set_value(45);
        v1.edges_begin()->edge().set_weight(45);

        for (auto e = v1.edges_begin(); e != v1.edges_end(); ++e)
            std::cout << e->source() << " " << e->target() << " " << e->edge().weight() << std::endl;
        for (auto e = v1.cedges_begin(); e != v1.cedges_end(); ++e)
            std::cout << e->source() << " " << e->target() << " " << e->edge().weight() << std::endl;

    }
    {
        auto gwl = create_adj_lists<Graph_type::DIGRAPH, int, int>(); // todo remove
        auto& v1 = gwl.create_vertex(1);
        auto& v2 = gwl.create_vertex(2);
        auto& v3 = gwl.create_vertex(3);
        gwl.add_edge(v1, v2, 4);
        gwl.add_edge(v1, v3, 8);

        // auto& v = *gwl.begin();
        // v.set_value(22);

        // v1.edges_begin();

        // std::cout << gwl.get_edge_weight(v1, v2) << std::endl;

        v1.edges_begin()->target().set_value(45);
        v1.edges_begin()->edge().set_weight(45);

        // for (auto e = v1.edges_begin(); e != v1.edges_end(); ++e)
        //     std::cout << e->source() << " " << e->target() << " " << e->edge().weight() << std::endl;
        // for (auto e = v1.cedges_begin(); e != v1.cedges_end(); ++e)
        //     std::cout << e->source() << " " << e->target() << " " << e->edge().weight() << std::endl;
    }
    std::cout << time(nullptr) << std::endl;
}
