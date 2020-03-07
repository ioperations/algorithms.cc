#include "adjacency_matrix.h"
#include "adjacency_lists.h"
#include "flow_network.h"
#include "graphs.h"

#include <string>
#include <stdexcept>
#include <sstream>

using namespace Graph;

template<typename P>
void print_path(const P& p) {
    print_collection(p.cbegin(), p.cend(), " - ", [](auto p) -> auto& { return *p; }, std::cout);
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
    print_path(h_path); // todo empty, fix

    std::cout << std::endl << "internal structure: " << std::endl;
    print_representation(graph, std::cout);

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
    print_representation(transitive_closure, std::cout);

    g = Samples::digraph_sample<G>();

    trace_dfs(g);
    std::cout << "DAG is valid: " << is_dag(g) << std::endl;

    std::cout << "topological sort (rearrange): " << std::endl << topological_sort_rearrange(g) << std::endl;
    std::cout << "topological sort (relabel): " << std::endl << topological_sort_relabel(g) << std::endl;

    g = Samples::dag_sample<G>();
    std::cout << "DAG is valid: " << is_dag(g) << std::endl;

    topological_sort_sinks_queue(g);

    g = Samples::strong_components_sample<decltype(g)>();

    std::cout << "dfs trace" << std::endl;
    trace_dfs(g);
    std::cout << "dfs with topological sort:" << std::endl;
    trace_dfs_topo_sorted(g);

    std::cout << "strong components (Kosaraju): " << std::endl << strong_components_kosaraju(g) << std::endl;
    std::cout << "strong components (Tarjan)" << std::endl << strong_components_tarjan(g) << std::endl;
}

template<typename G>
void test_weighted_graph() {
    auto g = Samples::weighted_graph_sample<G>();
    trace_dfs(pq_mst(g));

    g = Samples::spt_sample<G>();
    Spt spt(g, g.vertex_at(0), g.vertices_count());
    trace_dfs(compose_path_tree(g, spt.spt_.cbegin() + 1, spt.spt_.cend()));

    Full_spts full_spts(g, 1);
    auto diameter = full_spts.diameter();
    std::cout << *diameter.first << " " << *diameter.second << std::endl;

    std::cout << "diameter: " << *diameter.first;
    for (auto v = diameter.first; v != diameter.second; ) {
        v = full_spts.path(*v, *diameter.second).source_;
        std::cout << " - " << *v;
    }
    std::cout << " (" << full_spts.distance(*diameter.first, *diameter.second) << ")" << std::endl;
}

template<typename G>
struct Dag_full_spts {
    using vertex_t = typename G::vertex_type;
    using w_t = typename G::edge_type::value_type;
    using edge_t = typename G::vertex_type::const_edges_iterator::entry_type;
    const G& g_;
    Array<Array<w_t>> distances_;
    Array<Array<edge_t>> spts_;
    Dag_full_spts(const G& g, w_t max_weight) 
        :g_(g),
        distances_(g.vertices_count(), Array<w_t>(g.vertices_count(), max_weight)),
        spts_(g.vertices_count(), Array<edge_t>(g.vertices_count(), edge_t())) 
    {
        for (size_t i = 0; i < g.vertices_count(); ++i)
            if (!spts_[i][i].target_)
                foo(g.vertex_at(i));
    }
    void foo(const vertex_t& s) {
        for (auto e = s.cedges_begin(); e != s.cedges_end(); ++e) {
            auto& t = e->target();
            auto weight = e->edge().weight();
            if (distances_[s][t] > weight) {
                distances_[s][t] = weight;
                spts_[s][t] = *e;
            }
            if (!spts_[t][t].target_)
                foo(t);
            for (auto ii = g_.cbegin(); ii != g_.cend(); ++ii) {
                auto& i = *ii;
                auto distance = distances_[t][i] + weight;
                if (spts_[t][i].target_ && distances_[s][i] > distance) {
                    distances_[s][i] = distance;
                    spts_[s][i] = *e;
                }
            }
        }
    }
};

template<typename G>
void test_weighted_dag() {
    auto g = Samples::weighted_dag_sample<G>();
    validate_dag(g);

    Dag_lpt dag_lpt(g);
    auto lpt = compose_path_tree(g, dag_lpt.lpt_.cbegin(), dag_lpt.lpt_.cend());
    std::cout << "lpt:" << std::endl;
    trace_dfs(lpt);

    Dag_full_spts dd(g, g.vertices_count());
    std::cout << dd.distances_[0] << std::endl;
}

template<typename G>
struct Max_flow {
    using vertex_t = typename G::vertex_type;
    using w_t = typename G::edge_type::value_type;
    using edge_it_t = typename G::vertex_type::edges_iterator::entry_type;
    G& g_;
    vertex_t& s_;
    vertex_t& t_;
    Array<w_t> weights_;
    Array<typename vertex_t::edge_type::link_type*> links_;
    w_t sentinel_;
    Max_flow(G& g, vertex_t& s, vertex_t& t, w_t sentinel) 
        :g_(g), s_(s), t_(t), weights_(g.vertices_count()), links_(g.vertices_count()), sentinel_(sentinel)
    {
        while (pfs()) augment();
    }
    bool pfs() {
        Vertex_heap<vertex_t*, w_t> heap(g_.vertices_count(), weights_);
        for (auto v = g_.cbegin(); v != g_.cend(); ++v) {
            weights_[*v] = 0;
            links_[*v] = nullptr;
            heap.push(v);
        }
        weights_[s_] = -sentinel_;
        heap.move_up(&s_);

        while (!heap.empty()) {
            vertex_t& v = *heap.pop();

            weights_[v] = -sentinel_;
            if (v == t_ || (v != s_ && links_[v] == nullptr))
                break;

            for (auto e = v.edges_begin(); e != v.edges_end(); ++e) {
                auto link = e->edge().link();
                auto& w = link->other(v);
                auto cap = link->cap_r_to(w);
                auto p = cap < -weights_[v] ? cap : -weights_[v];
                if (cap > 0 && weights_[w] > -p) {
                    weights_[w] = -p;
                    heap.move_up(&w);
                    links_[w] = e->edge().link();
                }
            }
        }
        return links_[t_] != nullptr;
    }
    vertex_t& other_vertex(vertex_t& v) { return links_[v]->other(v); }
    void augment() {
        auto cap = links_[t_]->cap_r_to(t_);
        for (vertex_t* v = &other_vertex(t_); *v != s_;) {
            auto link = links_[*v];
            auto c = link->cap_r_to(*v);
            if (cap > c) 
                cap = c;
            v = &link->other(*v);
        }
        links_[t_]->add_flow_r_to(t_, cap);
        for (vertex_t* v = &other_vertex(t_); *v != s_; v = &other_vertex(*v))
            links_[*v]->add_flow_r_to(*v, cap);
    }
};

// todo rename edge.edge() to edge.link() ?

int main(int argc, char** argv) {
    test_graph<Adjacency_matrix<Graph_type::GRAPH, int>>("adjacency matrix");
    test_graph<Adjacency_lists<Graph_type::GRAPH, int>>("adjacency lists");

    {
        std::cout << "Warshall transitive closure" << std::endl;
        auto g = Samples::digraph_sample<Adjacency_matrix<Graph_type::DIGRAPH, int>>();
        auto transitive_closure = warshall_transitive_closure(g);
        print_representation(transitive_closure, std::cout);
    }

    test_digraph<Adjacency_matrix<Graph_type::DIGRAPH, int>>();
    test_digraph<Adjacency_lists<Graph_type::DIGRAPH, int>>();

    test_weighted_graph<Adjacency_matrix<Graph_type::GRAPH, int, double>>();
    test_weighted_graph<Adjacency_lists<Graph_type::GRAPH, int, double>>();

    test_weighted_dag<Adjacency_matrix<Graph_type::DIGRAPH, int, double>>();
    test_weighted_dag<Adjacency_lists<Graph_type::DIGRAPH, int, double>>();

    auto f = Samples::flow_sample();
    Max_flow m(f, f.vertex_at(0), f.vertex_at(5), f.vertices_count() * 10);
    std::cout << "max flow:" << std::endl;
    print_representation(f, std::cout);
}
