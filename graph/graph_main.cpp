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

template<typename G, typename V = typename G::Vertex>
G invert(const G& g) {
    G inverted;
    for (auto v = g.cbegin(); v != g.cend(); ++v)
        inverted.create_vertex(v->value());
    for (auto v = g.cbegin(); v != g.cend(); ++v)
        for (auto w = v->cbegin(); w != v->cend(); ++w)
            inverted.add_edge(*w, *v);
    return inverted;
}

template<typename G, typename V = typename G::Vertex>
void topological_sort(const G& g) {
    struct Searcher {
        const G& g_;
        Graph::Counters<int> pre_;
        Array<int> post_;
        Array<size_t> post_inverted_;
        int post_counter_;
        Searcher(const G& g, size_t size) 
            :g_(g), pre_(size), post_(size), post_inverted_(size), post_counter_(-1)
        {}
        void search() {
            for (auto v = g_.cbegin(); v != g_.cend(); ++v)
                if (pre_.is_unset(*v))
                    search(*v, *v);
        }
        void search(const V& v, const V& w) {
            pre_.set_next(w);
            for (auto it = w.cbegin(); it != w.cend(); ++it) {
                auto& t = *it;
                if (pre_.is_unset(t))
                    search(w, t);
            }
            post_[w] = ++post_counter_;
            post_inverted_[post_counter_] = w;
        }
    };
    std::cout << g.vertices_count() << std::endl;
    Searcher s(g, g.vertices_count());
    s.search();
    std::cout << s.post_ << std::endl;
    std::cout << s.post_inverted_ << std::endl;
}

int main() {


    // test_graph<Graph::Adjacency_matrix<int>>("adjacency matrix");
    // test_graph<Graph::Adjacency_lists<int>>("adjacency lists");

    // std::cout << std::endl;
    // test_digraph<Graph::Adjacency_matrix<int, Graph::Graph_type::DIGRAPH>>();
    // test_digraph<Graph::Adjacency_lists<int, Graph::Graph_type::DIGRAPH>>();

    // std::cout << "Warshall transitive closure" << std::endl;
    auto g = Graph::Samples::digraph_sample<Graph::Adjacency_matrix<int, Graph::Graph_type::DIGRAPH>>();
    // auto transitive_closure = Graph::warshall_transitive_closure(g);
    // transitive_closure.print_internal(std::cout);

    // Graph::trace_dfs(g);

    // std::cout << "DAG is valid: " << Graph::is_dag(g) << std::endl;

    g = Graph::Samples::dag_sample<Graph::Adjacency_matrix<int, Graph::Graph_type::DIGRAPH>>();

    Graph::trace_dfs(g);
    std::cout << "DAG is valid: " << Graph::is_dag(g) << std::endl;


    auto inverted = invert(g);
    Graph::trace_dfs(inverted);
    std::cout << "DAG is valid: " << Graph::is_dag(g) << std::endl;
    topological_sort(inverted);
}
