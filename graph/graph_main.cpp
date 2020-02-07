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
void mst_prim(const G& g) {
    using edge_type = typename G::edge_type;
    using vertex_type = typename G::vertex_type;
    struct Searcher {
        const G& g_;
        Array<double> weights_;
        Array<const edge_type*> fr_, mst_;

        Searcher(const G& g, size_t size) :g_(g), weights_(size), fr_(size), mst_(size) {
            weights_.fill(size); // todo why g size?
            fr_.fill(nullptr);
            mst_.fill(nullptr);
        }
        void search() {

            using edge_type_2 = typename G::vertex_type::const_edges_iterator::entry_type;
            Array<edge_type_2> frr(g_.vertices_count());
            Array<edge_type_2> mstt(g_.vertices_count());

            mstt.fill(edge_type_2());
            frr.fill(edge_type_2());

            for (const vertex_type* v = g_.cbegin(); v; ) {
                std::cout << "vertex: " << *v << std::endl;
                const vertex_type* next = nullptr;
                for (auto edge = v->cedges_begin(); edge != v->cedges_end(); ++edge) {
                    auto& w = edge->target();
                    if (mstt[w].target_ == nullptr) {
                        std::cout << edge->source() << " " << edge->target() << std::endl;
                        auto weight = edge->edge().weight();
                        if (weight < weights_[w]) {
                            weights_[w] = weight;
                            frr[w] = *edge;
                        }
                        if (next == nullptr || weights_[w] < weights_[*next])
                            next = &w;
                    }
                }
                if (next)
                    mstt[*next] = frr[*next];
                v = next;
            }

            for (auto mm : mstt) {
                if (mm.source_)
                    std::cout << mm.source() << " - " << mm.target();
                else
                    std::cout << "null";
                std::cout << "  ";
            }
            std::cout << std::endl;

            for (auto mm : frr) {
                if (mm.source_)
                    std::cout << mm.source() << " - " << mm.target();
                else
                    std::cout << "null";
                std::cout << "  ";
            }
            std::cout << std::endl;


            // size_t min = -1;
            // for (size_t v = 0; min != 0; v = min) {
            //     std::cout << "v = " << v << std::endl;
            //     min = 0;
            //     for (auto ww = g_.cbegin() + 1; ww != g_.cend(); ++ww) {
            //         if (mst_[*ww] == nullptr) {
            //         std::cout << "aaa " << *ww << std::endl;
            //             auto edge = g_.get_edge(g_.vertex_at(v), *ww);
            //             if (edge) {
            //                 std::cout << g_.vertex_at(v) << " - " << *ww << std::endl;
            //                 auto weight = edge->weight();
            //                 if (weight < weights_[*ww]) {
            //                     weights_[*ww] = weight;
            //                     fr_[*ww] = edge;
            //                 }
            //                 if (weights_[*ww] < weights_[min])
            //                     min = *ww;
            //             }
            //         }
            //     }
            //     if (min)
            //         mst_[min] = fr_[min];
            // }
        }
    };
    Searcher s(g, g.vertices_count());
    s.search();

    std::cout << s.weights_ << std::endl;
    std::cout << s.mst_ << std::endl;
    std::cout << s.fr_ << std::endl;
    for (auto e : s.mst_)
        if (e)
            std::cout << e->weight() << std::endl;
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

    auto g1 = Samples::weighted_graph_sample<Adjacency_lists<Graph_type::GRAPH, int, double>>();
    g1.print_internal(std::cout);

    mst_prim(g1);

    // auto g = Samples::euler_tour_sample<Adjacency_matrix<int>>();
    // dfs(g, [](auto& v) {
    //     std::cout << v << std::endl;
    // }, [](auto& v, auto& w) {
    //     std::cout << v << " " << w << std::endl;
    // });
}
