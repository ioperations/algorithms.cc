#include "adjacency_matrix.h"
#include "adjacency_lists.h"
#include "graphs.h"
#include "heap.h"

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

template<typename V, typename W>
class Vertex_heap : public Multiway_heap_base<V, Vertex_heap<V, W>> {
    private:
        using Base = Multiway_heap_base<V, Vertex_heap<V, W>>;
        Array<W>& weights_;
    public:
        Vertex_heap(size_t size, Array<W>& weights) :Base(size), weights_(weights) {}
        bool compare(const V& v1, const V& v2) { return weights_[*v1] > weights_[*v2]; }
        size_t get_index(const V& v) { return *v; }
};

template<typename G, typename E>
auto compose_path_tree(const G& g, const E& edges_b, const E& edges_e) {
    Adjacency_lists<Graph_type::DIGRAPH, typename G::vertex_type::value_type, typename G::edge_type::value_type> mst;
    for (auto v = g.cbegin(); v != g.cend(); ++v)
        mst.create_vertex(v->value());
    for (auto e = edges_b; e != edges_e; ++e)
        if (e->target_)
            mst.add_edge(mst.vertex_at(e->source()), mst.vertex_at(e->target()), e->edge().weight());
    return mst;
}

template<typename G>
auto pq_mst(const G& g) {
    using vertex_t = typename G::vertex_type;
    using w_t = typename G::edge_type::value_type;

    struct Pq_mst_searcher {
        const G& g_;
        Array<w_t> weights_;
        Array<typename G::vertex_type::const_edges_iterator::entry_type> fr_, mst_;
        Pq_mst_searcher(const G& g, size_t size) :g_(g), weights_(size), fr_(size), mst_(size)
        {
            for (auto& e : fr_) e.target_ = nullptr;
            for (auto& e : mst_) e.target_ = nullptr;
        }
        void search(const vertex_t& v) {
            Vertex_heap<const vertex_t*, w_t> heap(g_.vertices_count(), weights_);
            heap.push(&v);
            while (!heap.empty()) {
                const vertex_t& w = *heap.pop();
                mst_[w] = fr_[w];
                for (auto e = w.cedges_begin(); e != w.cedges_end(); ++e) {
                    const vertex_t& t = e->target();
                    w_t weight = e->edge().weight();
                    if (!fr_[t].target_) {
                        weights_[t] = weight;
                        heap.push(&t);
                        fr_[t] = *e;
                    } else if (!mst_[t].target_ && weight < weights_[t]) {
                        weights_[t] = weight;
                        heap.move_up(&w);
                        fr_[t] = *e;
                    }
                }
            }
        }
        void search() {
            for (auto v = g_.cbegin(); v != g_.cend(); ++v)
                if (!mst_[*v].target_)
                    search(*v);
        }
    };

    Pq_mst_searcher s(g, g.vertices_count());
    s.search();
    return compose_path_tree(g, s.mst_.cbegin(), s.mst_.cend());
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
struct Spt {
    using vertex_t = typename G::vertex_type;
    using edge_t = typename G::vertex_type::const_edges_iterator::entry_type;
    using weight_t = typename G::edge_type::value_type;

    Array<weight_t> weights_;
    Array<edge_t> spt_;
    Spt(const G& g, const vertex_t& vertex, weight_t max_weight) 
        :weights_(g.vertices_count()), spt_(g.vertices_count()) 
    {
        weights_.fill(max_weight);
        for (auto& e : spt_) e.target_ = nullptr;

        Vertex_heap<const vertex_t*, weight_t> heap(g.vertices_count(), weights_);
        for (const vertex_t* v = g.cbegin(); v != g.cend(); ++v)
            heap.push(v);
        weights_[vertex] = 0;
        heap.move_up(&vertex);
        while (!heap.empty()) {
            const vertex_t* v = heap.pop();
            if (v != &vertex && !spt_[*v].target_)
                return;
            for (auto e = v->cedges_begin(); e != v->cedges_end(); ++e) {
                const vertex_t* w = e->target_;
                weight_t weight = weights_[*v] + e->edge().weight();
                if (weight < weights_[*w]) {
                    weights_[*w] = weight;
                    heap.move_up(w);
                    spt_[*w] = *e;
                }
            }
        }
    }
};

template<typename G>
struct Full_spts {
    using vertex_t = typename G::vertex_type;
    using w_t = typename G::edge_type::value_type;
    using edge_t = typename G::vertex_type::const_edges_iterator::entry_type;
    const G& g_;
    Array<Array<w_t>> weights_;
    Array<Array<edge_t>> spts_;
    Full_spts(const G& g, w_t max_weight) 
        :g_(g), weights_(g.vertices_count()), spts_(g.vertices_count()) 
    {
        size_t i = 0;
        for (auto v = g.cbegin(); v != g.cend(); ++v, ++i) {
            Spt spt(g, *v, max_weight);
            weights_[i] = std::move(spt.weights_);
            spts_[i] = std::move(spt.spt_);
        }
    }
    w_t distance(size_t v, size_t w) { return weights_[v][w]; }
    const edge_t& path(size_t v, size_t w) { return spts_[w][v]; }
    const edge_t& path_r(size_t v, size_t w) { return spts_[v][w]; }
    std::pair<const vertex_t*, const vertex_t*> diameter() {
        size_t v_max = 0;
        size_t w_max = 0;
        for (auto v = g_.cbegin(); v != g_.cend(); ++v)
            for (auto w = g_.cbegin(); w != g_.cend(); ++w)
                if (path(*v, *w).target_ && distance(*v, *w) > distance(v_max, w_max)) {
                    v_max = *v;
                    w_max = *w;
                }
        return {&g_.vertex_at(v_max), &g_.vertex_at(w_max)};
    }
};

template<typename G>
struct Dag_lpt {
    using w_t = typename G::edge_type::value_type;
    using edge_t = typename G::vertex_type::const_edges_iterator::entry_type;
    Array<w_t> weights_;
    Array<edge_t> lpt_;
    Dag_lpt(const G& g) :weights_(g.vertices_count()), lpt_(g.vertices_count()) {
        weights_.fill(0);
        for (auto& e : lpt_) e.target_ = nullptr;

        Topological_sorter sorter(g);
        sorter.search();
        auto& t_sorted = sorter.post_i_;

        for (auto i = t_sorted.crbegin(); i != t_sorted.crend(); ++i) {
            auto& v = g.vertex_at(*i);
            for (auto e = v.cedges_begin(); e != v.cedges_end(); ++e) {
                auto& w = e->target();
                auto distance = weights_[v] + e->edge().weight();
                if (weights_[w] < distance) {
                    weights_[w] = distance;
                    lpt_[w] = *e;
                }
            }
        }
    }
};

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
void test_weighted_dag() {
    auto g = Samples::weighted_dag_sample<G>();
    validate_dag(g);

    Dag_lpt dag_lpt(g);
    auto lpt = compose_path_tree(g, dag_lpt.lpt_.cbegin(), dag_lpt.lpt_.cend());
    std::cout << "lpt:" << std::endl;
    trace_dfs(lpt);
}

int main() {
    test_graph<Adjacency_matrix<Graph_type::GRAPH, int>>("adjacency matrix");
    test_graph<Adjacency_lists<Graph_type::GRAPH, int>>("adjacency lists");

    {
        std::cout << "Warshall transitive closure" << std::endl;
        auto g = Samples::digraph_sample<Adjacency_matrix<Graph_type::DIGRAPH, int>>();
        auto transitive_closure = warshall_transitive_closure(g);
        transitive_closure.print_internal(std::cout);
    }

    test_digraph<Adjacency_matrix<Graph_type::DIGRAPH, int>>();
    test_digraph<Adjacency_lists<Graph_type::DIGRAPH, int>>();

    test_weighted_graph<Adjacency_matrix<Graph_type::GRAPH, int, double>>();
    test_weighted_graph<Adjacency_lists<Graph_type::GRAPH, int, double>>();

    test_weighted_dag<Adjacency_matrix<Graph_type::DIGRAPH, int, double>>();
    test_weighted_dag<Adjacency_lists<Graph_type::DIGRAPH, int, double>>();
}
