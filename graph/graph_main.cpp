#include "adjacency_matrix.h"
#include "adjacency_lists.h"
#include "network_flow.h"
#include "network_flow_simplex.h"
#include "graphs.h"
#include "array_queue.h"

#include <string>
#include <stdexcept>
#include <sstream>

using namespace Graph;

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
    Spt spt(g, g[0], g.vertices_count());
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
    static auto empty_edge_it() {
        auto create = []() {
            edge_t e;
            e.target_ = nullptr;
            return e;
        };
        static edge_t e(create());
        return e;
    }
    const G& g_;
    Array<Array<w_t>> distances_;
    Array<Array<edge_t>> spts_;
    Dag_full_spts(const G& g, w_t max_weight) 
        :g_(g),
        distances_(g.vertices_count(), Array<w_t>(g.vertices_count(), max_weight)),
        spts_(g.vertices_count(), Array<edge_t>(g.vertices_count(), empty_edge_it())) 
    {
        for (size_t i = 0; i < g.vertices_count(); ++i)
            if (!spts_[i][i].target_)
                dfs(g[i]);
    }
    void dfs(const vertex_t& s) {
        for (auto e = s.cedges_begin(); e != s.cedges_end(); ++e) {
            auto& t = e->target();
            auto weight = e->edge().weight();
            if (distances_[s][t] > weight) {
                distances_[s][t] = weight;
                spts_[s][t] = *e;
            }
            if (!spts_[t][t].target_)
                dfs(t);
            for (auto it = g_.cbegin(); it != g_.cend(); ++it) {
                auto& i = *it;
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
    Array<typename G::link_type*> links_;
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

template<typename G>
class Pre_flow_push_max_flow {
    private:
        using vertex_t = typename G::vertex_type;
        using w_t = typename G::edge_type::value_type;
        using edge_it_t = typename G::vertex_type::edges_iterator::entry_type;
        G& g_;
        vertex_t& s_;
        vertex_t& t_;
        const size_t v_count_;
        Array<size_t> heights_;
        Array<w_t> weights_;
        inline void init_heights() {
        }
    public:
        Pre_flow_push_max_flow(G& g, vertex_t& s, vertex_t& t, w_t sentinel) 
            :g_(g), s_(s), t_(t), v_count_(g.vertices_count()), heights_(v_count_, v_count_ + 1), weights_(v_count_, 0) 
        {
            Array_queue<vertex_t*> queue(v_count_);
            queue.push(&t_);
            heights_[t_] = 0;
            auto default_height = v_count_ + 1;
            while (!queue.empty()) {
                auto& v = *queue.pop();
                auto height = heights_[v] + 1;
                for (auto e = v.edges_begin(); e != v.edges_end(); ++e) {
                    auto link = e->edge().link();
                    auto& w = link->other(v);
                    if (heights_[w] == default_height && link->is_from(w)) {
                        heights_[w] = height;
                        queue.push(&w);
                    }
                }
            }

            queue.push(&s);
            weights_[t] = -(weights_[s] = sentinel * v_count_);

            while (!queue.empty()) {
                auto& v = *queue.pop();
                for (auto e = v.edges_begin(); e != v.edges_end(); ++e) {
                    auto link = e->edge().link();
                    auto& w = link->other(v);
                    auto p = link->cap_r_to(w);
                    if (p > weights_[v])
                        p = weights_[v];
                    if ((p > 0 && v == s) || heights_[v] == heights_[w] + 1) {
                        link->add_flow_r_to(w, p);
                        weights_[v] -= p;
                        weights_[w] += p;
                        if (w != s && w != t)
                            queue.push(&w);
                    }
                }
                if (v != s && v != t && weights_[v] > 0) {
                    ++heights_[v];
                    queue.push(&v);
                }
            }
            std::cout << heights_ << std::endl;
        }
};

template<typename F, typename M>
void find_feasible_flow(F& f, const M& supply, const M& demand) {
    auto& s = f.create_vertex(-1);
    auto& t = f.create_vertex(-1);

    for (auto e = supply.cbegin(); e != supply.cend(); ++e)
        f.add_edge(s, f[e->first], e->second, 0);
    for (auto e = demand.cbegin(); e != demand.cend(); ++e)
        f.add_edge(f[e->first], t, e->second, 0);

    Pre_flow_push_max_flow m(f, s, t, f.vertices_count() * 10);
    std::cout << "feasible flow, demands met: " << std::endl;
    for (auto e = t.cedges_begin(); e != t.cedges_end(); ++e) {
        auto& link = *e->edge().link();
        std::cout << link.other(t) << ": " << link.flow() << "/" << link.cap() << "; ";
    }
    std::cout << std::endl;
}

template<typename M>
auto bipartite_matching(const M& mapping) {
    using value_type = typename M::key_type;

    Builder<Network_flow<value_type, int>> b;
    for (auto e = mapping.cbegin(); e != mapping.cend(); ++e)
        b.for_vertex(e->first);
    for (auto e = mapping.cbegin(); e != mapping.cend(); ++e) {
        auto v = b.for_vertex(e->first);
        auto& list = e->second;
        for (auto t = list.cbegin(); t != list.cend(); ++t)
            v.add_edge(*t, 1, 0);
    }
    auto f = b.build();

    static const value_type default_value = value_type();
    auto& s = f.create_vertex(default_value);
    size_t i = 0;
    for (; i < mapping.size(); ++i)
        f.add_edge(s, f[i], 1, 0);

    auto& t = f.create_vertex(default_value);
    for (; i < f.vertices_count(); ++i)
        f.add_edge(f[i], t, 1, 0);

    Max_flow m(f, s, t, f.vertices_count() * 10);

    std::map<value_type, value_type> result;
    for (i = 0; i < mapping.size(); ++i) {
        auto& v = f[i];
        for (auto e = v.cedges_begin(); e != v.cedges_end(); ++e) {
            auto& link = *e->edge().link();
            if (v == link.source() && link.flow() > 0) {
                result[v.value()] = link.other(v).value();
                break;
            }
        }
    }
    return result;
}

/**
 * Bellman-Ford algorithm for negative cycles search.
 */
template<typename G>
Array_cycle find_negative_cycle(const G& g, const typename G::vertex_type& s, 
                                typename G::edge_type::value_type sentinel) {
    using vertex_type = typename G::vertex_type;
    using w_t = typename G::edge_type::value_type;
    using edge_t = typename G::vertex_type::const_edges_iterator::entry_type;

    Array<w_t> weights(g.vertices_count(), sentinel);
    Array<edge_t> spt(g.vertices_count());
    for (auto& s : spt)
        s.target_ = nullptr;

    weights[s] = 0;

    Forward_list<const vertex_type*> queue;
    queue.push_back(&s);
    queue.push_back(nullptr);

    size_t n = 0;

    bool completed = false;
    while (!completed && !queue.empty()) {
        const vertex_type* v;
        while (!completed && (v = queue.pop_front()) == nullptr) {
            completed = n++ > g.vertices_count();
            if (!completed)
                queue.push_back(nullptr);
        }
        if (!completed)
            for (auto e = v->cedges_begin(); e != v->cedges_end(); ++e) {
                auto& w = e->target();
                auto weight = weights[*v] + e->edge().weight();
                if (weights[w] > weight) {
                    weights[w] = weight;
                    queue.push_back(&w);
                    spt[w] = *e;
                }
            }
    }

    using cycle_g_type = Adjacency_lists<Graph_type::DIGRAPH, int>;
    cycle_g_type gg;
    for (auto v = g.cbegin(); v != g.cend(); ++v)
        gg.create_vertex(*v);

    for (auto& s : spt)
        if (s.target_)
            gg.add_edge(gg[s.source()], gg[s.target()]);

    struct Searcher : public Post_dfs_base<cycle_g_type, size_t, size_t, Searcher> {
        using Base = Post_dfs_base<cycle_g_type, size_t, size_t, Searcher>;
        bool found_;
        Array<size_t> cycle_;
        Searcher(const cycle_g_type& g, w_t sentinel) :Base(g), found_(false), cycle_(g.vertices_count(), sentinel) {}
        void search_vertex(const typename Base::vertex_type& v) {
            if (!found_)
                Base::search_vertex(v);
        }
        void visit_edge(const typename Base::edge_type& e) {
            auto& v = e.source();
            auto& w = e.target();
            if (!found_ && Base::pre_[w] < Base::pre_[v] && Base::post_.is_unset(w))
                found_ = true;
            if (found_)
                cycle_[v] = w;
        }
    };
    Searcher searcher(gg, sentinel);
    searcher.search();
    if (searcher.found_)
        return {std::move(searcher.cycle_), static_cast<size_t>(sentinel)};
    else
        return {{}, static_cast<size_t>(sentinel)};
}

int main(int argc, char** argv) {
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

    {
        auto f = Samples::flow_sample();
        Max_flow m(f, f[0], f[5], f.vertices_count() * 10);
        std::cout << "max flow:" << std::endl;
        print_representation(f, std::cout);
    }
    std::cout << std::endl;
    {
        auto f = Samples::flow_sample();
        Pre_flow_push_max_flow m(f, f[0], f[5], f.vertices_count() * 10);
        std::cout << "pre flow push max flow:" << std::endl;
        print_representation(f, std::cout);
    }

    std::cout << std::endl;

    Builder<Adjacency_lists<Graph_type::DIGRAPH, int, int>> b;
    for (int i = 0; i < 6; ++i) b.for_vertex(i);

    auto g = b
        .for_vertex(0).add_edge(1, 2).add_edge(2, 3)
        .for_vertex(1).add_edge(2, 3).add_edge(4, 2)
        .for_vertex(2).add_edge(3, 2).add_edge(4, 1)
        .for_vertex(3).add_edge(1, 3).add_edge(5, 2)
        .for_vertex(4).add_edge(3, 3).add_edge(5, 3)
        .build();

    print_representation(g, std::cout);

    {
        auto f = Samples::flow_sample();
        find_feasible_flow(f,
                           std::map<int, int>{{0, 3}, {1, 3}, {3, 1}},
                           std::map<int, int>{{2, 1}, {4, 1}, {5, 5}});
    }
    {
        std::map<int, Forward_list<int>> mapping = {
            {0, {6, 7, 8}},
            {1, {6, 7, 11}},
            {2, {8, 9, 10}},
            {3, {6, 7}},
            {4, {9, 10, 11}},
            {5, {8, 10, 11}}
        };
        std::cout << "bipartite matching: " << std::endl;
        for (auto e : bipartite_matching(mapping))
            std::cout << e.first << ": " << e.second << ", ";
        std::cout << std::endl;
    }
    {
        Builder<Adjacency_lists<Graph_type::DIGRAPH, int, double>> b;
        for (int i = 0; i < 6; ++i) b.for_vertex(i);
        auto g = b
            .for_vertex(0).add_edge(1, .41).add_edge(5, .29)
            .for_vertex(1).add_edge(2, .51).add_edge(4, .28)
            // .for_vertex(1).add_edge(2, .51).add_edge(4, .32)
            .for_vertex(2).add_edge(3, .50)
            .for_vertex(3).add_edge(0, .45).add_edge(5, -.38)
            .for_vertex(4).add_edge(3, .36)
            .for_vertex(5).add_edge(1, -.29).add_edge(4, .21)
            .build();

        auto n_cycle = find_negative_cycle(g, g[0], 200);
        if (!n_cycle.empty()) {
            auto v = n_cycle.cbegin();
            auto first = v;
            do {
                std::cout << *v << " - ";
                ++v;
            } while (v != first);
            std::cout << std::endl;
        }
    }
}
