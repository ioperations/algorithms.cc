#pragma once

#include <iostream>
#include <map>

#include "adjacency_lists.h"
#include "array.h"
#include "dfs.h"
#include "stack.h"
#include "string_utils.h"
#include "two_dimensional_array.h"

namespace Graph {

template <typename G, typename T = typename G::vertex_type::value_type>
class Constructor {
   private:
    using vertex_type = typename G::vertex_type;
    G& m_graph;
    std::map<T, typename G::vertex_type&> m_vertices;

   public:
    Constructor(G& graph) : m_graph(graph) {}
    Constructor& add_edge(const T& l1, const T& l2) {
        auto& v1 = get_or_create_vertex(l1);
        auto& v2 = get_or_create_vertex(l2);
        m_graph.add_edge(v1, v2);
        return *this;
    }
    vertex_type& get_or_create_vertex(const T& l) {
        auto it = m_vertices.find(l);
        if (it == m_vertices.end()) {
            auto& v = m_graph.create_vertex(l);
            m_vertices.insert({l, v});
            return v;
        }
        return it->second;
    }
    vertex_type& get_vertex(const T& label) {
        auto it = m_vertices.find(label);
        if (it == m_vertices.end())
            throw std::runtime_error("vertex "_str + label + " not found");
        return it->second;
    };
};

template <typename G>
class Builder {
   private:
    class Vertex;
    friend class Vertex;
    using vertex_type = typename G::vertex_type;
    using value_type = typename vertex_type::value_type;

    G m_g;
    std::map<value_type, vertex_type* const> m_map;

    vertex_type* const get_or_create_vertex(const value_type& value) {
        auto vertex = m_map.find(value);
        if (vertex == m_map.end()) {
            auto p = &m_g.create_vertex(value);
            m_map.insert({value, p});
            return p;
        }
        return vertex->second;
    }
    template <typename... Args>
    void add_edge(vertex_type* const v, const value_type& w, Args&&... args) {
        m_g.add_edge(*v, *get_or_create_vertex(w), std::forward<Args>(args)...);
    }
    template <typename E, typename... Es>
    void add_edges(Vertex& vertex, E&& edge, Es&&... edges) {
        m_g.add_edge(*vertex.m_vertex,
                     *get_or_create_vertex(std::forward<E>(edge)));
        add_edges(vertex, std::forward<Es>(edges)...);
    }
    void add_edges(Vertex&) {}

   public:
    Vertex for_vertex(const value_type& value) {
        return Vertex(*this, get_or_create_vertex(value));
    }
    G build() { return std::move(m_g); }
};

template <typename G>
class Builder<G>::Vertex {
   private:
    using builder_type = Builder<G>;
    using vertex_type = typename G::vertex_type;
    using value_type = typename vertex_type::value_type;

    builder_type& m_builder;

   public:
    vertex_type* const m_vertex;
    Vertex(builder_type& b, vertex_type* const v) : m_builder(b), m_vertex(v) {}

    template <typename... Args>
    Vertex& add_edge(const value_type& v, Args&&... args) {
        m_builder.add_edge(m_vertex, v, std::forward<Args>(args)...);
        return *this;
    }
    template <typename... E>
    Vertex& add_edges(E&&... edges) {
        m_builder.add_edges(*this, std::forward<E>(edges)...);
        return *this;
    }
    Vertex for_vertex(const value_type& v) { return m_builder.for_vertex(v); }
    G build() { return m_builder.build(); }
};

template <typename It, typename F>
void print_collection(const It& b, const It& e, const char* del, F f,
                      std::ostream& stream) {
    if (b == e) return;
    auto item = b;
    stream << f(*item);
    for (++item; item != e; ++item) stream << del << f(*item);
}

template <typename It>
void print_collection(const It& b, const It& e, const char* del,
                      std::ostream& stream) {
    print_collection(
        b, e, del, [](auto& v) { return v; }, stream);
}

template <typename T>
class FixedSizeStack {
   private:
    Array<T> m_array;
    size_t m_index;

   public:
    FixedSizeStack(size_t size) : m_array(size), m_index(0) {}
    void push(const T& t) { m_array[m_index++] = t; }
    Array<T> to_reversed_array() {
        for (size_t i = 0; i < m_index / 2; ++i)
            std::swap(m_array[i], m_array[m_index - i - 1]);
        return std::move(m_array);
    }
};

template <typename V>
auto count_vertex_edges(const V& v) {
    size_t count = 0;
    for (auto e = v.cbegin(); e != v.cend(); ++e) ++count;
    return count;
}

template <typename G, typename V = typename G::vertex_type>
bool has_simple_path(const G& graph, const V& v1, const V& v2) {
    Array<bool> visited(graph.vertices_count());
    for (auto& b : visited) b = false;
    static struct {
        bool has_simple_path(const G& graph, const V& v1, const V& v2,
                             Array<bool>& visited) {
            if (v1 == v2) return true;
            visited[v1] = true;
            for (auto v = v1.cbegin(); v != v1.cend(); ++v)
                if (!visited[*v])
                    if (has_simple_path(graph, *v, v2, visited)) return true;
            return false;
        }
    } helper;
    return helper.has_simple_path(graph, v1, v2, visited);
}

template <typename G, typename V = typename G::vertex_type>
Array<const V*> compose_hamilton_path(const G& graph, const V& s, const V& t) {
    if (graph.vertices_count() < 1) return {};
    if (graph.vertices_count() < 2) return {&graph[0]};
    struct Helper {
        Array<bool> m_visited;
        FixedSizeStack<const V*> m_stack;
        const size_t m_size;
        Helper(size_t size)
            : m_visited(size, false), m_stack(size), m_size(size) {}
        bool compose_path(const V& s, const V& t) {
            return compose_path(s, t, m_size - 1);
        }
        bool compose_path(const V& s, const V& t, size_t depth) {
            bool has;
            if (s == t)
                has = depth == 0;
            else {
                has = false;
                m_visited[s] = true;
                for (auto v = s.cbegin(); v != s.cend() && !has; ++v)
                    has = !m_visited[*v] && compose_path(*v, t, depth - 1);
                if (!has) m_visited[s] = false;
            }
            if (has) m_stack.push(&s);
            return has;
        }
    };
    Array<const V*> path;
    Helper helper(graph.vertices_count());
    if (helper.compose_path(s, t))
        path = std::move(helper.m_stack.to_reversed_array());
    return path;
}

template <typename G, typename V = typename G::vertex_type>
auto compose_euler_tour(const G& g, const V& s) {
    using Path = Forward_list<const V*>;
    Path path;
    for (auto w = g.cbegin(); w != g.cend(); ++w)
        if (count_vertex_edges(*w) % 2 != 0) return path;

    struct Helper {
        const G& m_original_g;
        G& m_g;
        Stack<V*> m_stack;
        Path& m_path;
        Helper(const G& original_graph, G& g, Path& path)
            : m_original_g(original_graph), m_g(g), m_path(path) {}
        V* tour(V* v) {
            while (true) {
                auto it = v->begin();
                if (it == v->end()) break;
                m_stack.push(v);
                auto& w = *it;
                m_g.remove_edge(*v, w);
                v = &w;
            }
            return v;
        }
        void compose(V* v) {
            push_original_v(v);
            while (tour(v) == v && !m_stack.empty()) {
                v = m_stack.pop();
                push_original_v(v);
            }
        }
        void push_original_v(const V* v) {
            m_path.push_back(&m_original_g[*v]);
        }
    };
    auto g_copy = g;
    Helper(g, g_copy, path).compose(&g_copy[s]);
    return path;
}

template <typename G, typename V = typename G::vertex_type>
Forward_list<std::pair<const V*, const V*>> find_bridges(const G& graph) {
    struct Searcher {
        const G& m_g;
        Array<int> m_orders;
        Array<int> m_mins;
        int m_order;
        Forward_list<std::pair<const V*, const V*>> m_bridges;
        Searcher(const G& g)
            : m_g(g),
              m_orders(g.vertices_count()),
              m_mins(g.vertices_count()),
              m_order(-1) {
            for (auto& o : m_orders) o = -1;
        }
        void search() {
            for (auto v = m_g.cbegin(); v != m_g.cend(); ++v)
                if (m_orders[*v] == -1) search(*v, *v);
        }
        void search(const V& v, const V& w) {
            m_orders[w] = ++m_order;
            m_mins[w] = m_orders[w];
            for (auto t = w.cbegin(); t != w.cend(); ++t)
                if (m_orders[*t] == -1) {
                    search(w, *t);
                    if (m_mins[w] > m_mins[*t]) m_mins[w] = m_mins[*t];
                    if (m_mins[*t] == m_orders[*t])
                        m_bridges.emplace_back(&w, &*t);
                } else if (v != *t)
                    m_mins[w] = m_mins[*t];
        }
    };
    Searcher s(graph);
    s.search();
    return std::move(s.m_bridges);
}

template <typename V>
class ShortestPathsMatrix {
   public:
    using parents_type = Two_dimensional_array<const V*>;

   private:
    parents_type m_parents;

   public:
    ShortestPathsMatrix(parents_type&& parents) : m_parents(parents) {}
    auto find_path(const V& v, const V& w) {
        Forward_list<const V*> path;
        auto row = m_parents[w];
        for (auto t = &v; t != &w; t = row[*t]) path.push_back(t);
        path.push_back(&w);
        return path;
    }
};

template <typename G, typename V = typename G::vertex_type>
ShortestPathsMatrix<V> find_shortest_paths(const G& g) {
    typename ShortestPathsMatrix<V>::parents_type all_parents(
        g.vertices_count(), g.vertices_count());
    all_parents.fill(nullptr);
    for (auto v = g.cbegin(); v != g.cend(); ++v) {
        auto parents = all_parents[*v];
        Forward_list<std::pair<const V&, const V&>> queue;
        queue.emplace_back(*v, *v);
        while (!queue.empty()) {
            auto e = queue.pop_front();
            auto& parent = parents[e.second];
            if (!parent) {
                parent = &e.first;
                for (auto t = e.second.cbegin(); t != e.second.cend(); ++t)
                    if (!parents[*t]) queue.emplace_back(e.second, *t);
            }
        }
    }
    return {std::move(all_parents)};
}

template <typename G>
G warshall_transitive_closure(const G& g) {
    G g_copy = g;
    for (auto& v : g_copy) g_copy.add_edge(v, v);
    for (auto& i : g_copy)
        for (auto& s : g_copy)
            if (g_copy.has_edge(s, i))
                for (auto& t : g_copy)
                    if (g_copy.has_edge(i, t)) g_copy.add_edge(s, t);
    return g_copy;
}

template <typename G, typename V = typename G::vertex_type>
G dfs_transitive_closure(const G& g) {
    struct Helper {
        G& m_g;
        Array<bool> m_a;
        Helper(G& g) : m_g(g), m_a(g.vertices_count()) {}
        void search() {
            for (auto& v : m_g) {
                m_a.fill(false);
                search(v, v, v);
            }
        }
        void search(V& vv, V& v, V& w) {
            if (!m_a[w]) {
                m_a[w] = true;
                if (!m_g.has_edge(vv, w)) m_g.add_edge(vv, w);
                for (auto& t : w) search(vv, w, t);
            }
        }
    };
    auto g_copy = g;
    Helper(g_copy).search();
    return g_copy;
}

template <typename G, typename D>
class DfsTracerBase : public Post_dfs_base<G, size_t, size_t, D> {
   private:
    int m_depth;
    Stack<std::string> m_lines_stack;

   protected:
    std::ostream& m_stream;
    using Base = Post_dfs_base<G, size_t, size_t, D>;
    using vertex_type = typename Base::vertex_type;
    using edge_type = typename Base::edge_type;
    size_t m_last_root_id;

   public:
    DfsTracerBase(const G& g, std::ostream& stream)
        : Base(g), m_depth(-1), m_stream(stream), m_last_root_id(-1) {}
    void print_stack() {
        while (!m_lines_stack.empty())
            m_stream << m_lines_stack.pop() << std::endl;
    }
    void search() {
        Base::search();
        m_stream << m_last_root_id << std::endl;
        print_stack();
    }
    void visit_vertex(const vertex_type& v) {
        ++m_depth;
        if (m_depth == 0) {
            if (m_lines_stack.empty()) {
                m_stream << v << std::endl;
            } else
                print_stack();
            m_last_root_id = v;
        }
    }
    void visit_edge(const edge_type& e) {
        auto& v = e.source();
        auto& w = e.target();
        std::string str;
        if (Base::m_pre[w] > Base::m_pre[v])
            str = "down";
        else if (Base::m_post.is_unset(w))
            str = "back";
        else
            str = "cross";
        m_lines_stack.push(compose_stack_line(str, e));
    }
    void search_post_process(const vertex_type& v) {
        --m_depth;
        Base::search_post_process(v);
    }
    template <typename E, typename ET = typename E::edge_type::value_type>
    struct EdgeAppender {
        static void append(std::ostream& stream, const E& e) {
            stream << " (" << e.edge().weight() << ")";
        }
    };
    template <typename E>
    struct EdgeAppender<E, bool> {
        static void append(std::ostream& stream, const E& e) {}
    };
    std::string compose_stack_line(const std::string label,
                                   const edge_type& e) {
        std::stringstream ss;
        for (int i = 0; i < m_depth; ++i) ss << " ";
        ss << " " << e.target();
        EdgeAppender<edge_type>::append(ss, e);
        ss << " (" << label << ")";
        return ss.str();
    }
};

template <typename G>
void trace_dfs(const G& g, std::ostream& stream) {
    class Tracer : public DfsTracerBase<G, Tracer> {
       public:
        Tracer(const G& g, std::ostream& stream)
            : DfsTracerBase<G, Tracer>(g, stream) {}
    };
    Tracer(g, stream).search();
}

template <typename G, typename D>
struct InverterBase {
    G invert(const G& g) {
        G inverted;
        for (auto v = g.cbegin(); v != g.cend(); ++v)
            inverted.create_vertex(v->value());
        static_cast<D*>(this)->do_invert(g, inverted);
        return inverted;
    }
};

template <typename G, typename E = typename G::edge_type::value_type>
struct Inverter : public InverterBase<G, Inverter<G, E>> {
    void do_invert(const G& g, G& inverted) {
        for (auto v = g.cbegin(); v != g.cend(); ++v)
            for (auto e = v->cedges_begin(); e != v->cedges_end(); ++e)
                inverted.add_edge(inverted[e->target()], inverted[*v],
                                  e->edge().weight());
    }
};

template <typename G>
struct Inverter<G, bool> : public InverterBase<G, Inverter<G, bool>> {
    void do_invert(const G& g, G& inverted) {
        for (auto v = g.cbegin(); v != g.cend(); ++v)
            for (auto w = v->cbegin(); w != v->cend(); ++w)
                inverted.add_edge(inverted[*w], inverted[*v]);
    }
};

template <typename G>
G invert(const G& g) {
    static Inverter<G> inverter;
    return inverter.invert(g);
}

template <typename G>
Array<size_t> topological_sort_rearrange(const G& g) {
    auto inverted = invert(g);
    Post_dfs<G, size_t, size_t> d(inverted);
    d.search();
    return d.m_post.to_array();
}

template <typename G>
struct TopologicalSorter
    : public Post_dfs_base<G, size_t, size_t, TopologicalSorter<G>> {
    using Base = Post_dfs_base<G, size_t, size_t, TopologicalSorter<G>>;
    Array<size_t> m_post_i;
    TopologicalSorter(const G& g) : Base(g), m_post_i(g.vertices_count()) {}
    void search_post_process(const typename Base::vertex_type& v) {
        Base::search_post_process(v);
        m_post_i[Base::m_post[v]] = v;
    }
};

template <typename G>
Array<size_t> topological_sort_relabel(const G& g) {
    auto inverted = invert(g);
    TopologicalSorter s(inverted);
    s.search();
    return std::move(s.m_post_i);
}

template <typename G>
void trace_dfs_topo_sorted(const G& g, std::ostream& stream) {
    class Tracer : public DfsTracerBase<G, Tracer> {
       public:
        using Base = DfsTracerBase<G, Tracer>;
        using vertex_type = typename Base::vertex_type;
        Tracer(const G& g, std::ostream& stream) : Base(g, stream) {}
        void search() {
            auto t_order = topological_sort_relabel(Base::m_g);
            for (auto it = Base::m_g.crbegin(); it != Base::m_g.crend(); ++it) {
                const vertex_type& v = Base::m_g[t_order[*it]];
                if (Base::m_pre.is_unset(v)) Base::search_vertex(v);
            }
            Base::m_stream << Base::m_last_root_id << std::endl;
            Base::print_stack();
        }
    };
    Tracer(g, stream).search();
}

template <typename G>
void trace_bfs(const G& g) {
    using V = typename G::vertex_type;
    Counters<bool> pre(g.vertices_count());
    for (auto vv = g.cbegin(); vv != g.cend(); ++vv) {
        if (pre.is_unset(*vv)) {
            struct Entry {
                const V* const m_v;
                const int m_d;
                Entry(const V* const v, int d) : m_v(v), m_d(d) {}
            };
            Forward_list<Entry> queue;
            queue.emplace_back(&*vv, 0);
            while (!queue.empty()) {
                auto p = queue.pop_front();
                auto& w = *p.m_v;
                for (auto t = w.cbegin(); t != w.cend(); ++t) {
                    if (pre.is_unset(*t)) {
                        pre.set_next(*t);
                        queue.emplace_back(&*t, p.m_d + 1);
                    }
                    if (w != *t) {
                        for (int i = 0; i < p.m_d; ++i) std::cout << " ";
                        std::cout << w << " " << *t << std::endl;
                    }
                }
            }
        }
    }
}

class InvalidDagException : public std::runtime_error {
   public:
    InvalidDagException() : std::runtime_error("invalid dag") {}
};

template <typename G>
void validate_dag(const G& g) {
    struct Searcher : public Post_dfs_base<G, size_t, size_t, Searcher> {
        using Base = Post_dfs_base<G, size_t, size_t, Searcher>;
        Searcher(const G& g) : Base(g) {}
        void visit_edge(const typename Base::edge_type& e) {
            auto& v = e.source();
            auto& w = e.target();
            if (Base::m_pre[w] < Base::m_pre[v] && Base::m_post.is_unset(w))
                throw InvalidDagException();  // back link found
        }
    };
    Searcher(g).search();
}

template <typename G>
bool is_dag(const G& g) {
    try {
        validate_dag(g);
        return true;
    } catch (const InvalidDagException&) {
        return false;
    }
}

template <typename G, typename V = typename G::vertex_type>
auto topological_sort_sinks_queue(const G& g) {
    Array<size_t> in(g.vertices_count());
    in.fill(0);
    for (auto v = g.cbegin(); v != g.cend(); ++v)
        for (auto w = v->cbegin(); w != v->cend(); ++w) ++in[*w];
    Forward_list<const V* const> queue;
    for (auto v = g.cbegin(); v != g.cend(); ++v)
        if (in[*v] == 0) queue.push_back(&*v);
    Array<size_t> ordered(g.vertices_count());
    for (auto* ordered_it = ordered.begin(); !queue.empty(); ++ordered_it) {
        auto v = queue.pop_front();
        *ordered_it = *v;
        for (auto w = v->cbegin(); w != v->cend(); ++w)
            if (--in[*w] == 0) queue.push_back(&*w);
    }
    return ordered;
}

template <typename G, typename V = typename G::vertex_type>
Array<size_t> strong_components_kosaraju(const G& g) {
    struct Searcher {
        const G& m_g;
        Array<size_t> m_ids;
        size_t m_group_id;
        Searcher(const G& g)
            : m_g(g), m_ids(g.vertices_count()), m_group_id(-1) {
            m_ids.fill(-1);
        }
        void search() {
            auto t_order = topological_sort_relabel(m_g);
            for (auto v = m_g.crbegin(); v != m_g.crend(); ++v) {
                auto& t = m_g[t_order[*v]];
                if (m_ids[t] == static_cast<size_t>(-1)) {
                    ++m_group_id;
                    search(t);
                }
            }
        }
        void search(const V& v) {
            m_ids[v] = m_group_id;
            for (auto w = v.cbegin(); w != v.cend(); ++w)
                if (m_ids[*w] == static_cast<size_t>(-1)) search(*w);
        }
    };

    Searcher s(g);
    s.search();
    return std::move(s.m_ids);
}

template <typename G>
Array<size_t> strong_components_tarjan(const G& g) {
    using V = typename G::vertex_type;
    struct Searcher {
        const G& m_g;
        Counters<size_t> m_pre;
        Array<size_t> m_min;
        Array<size_t> m_ids;
        size_t m_group_id;
        Stack<const V* const> m_stack;
        Searcher(const G& g)
            : m_g(g),
              m_pre(g.vertices_count()),
              m_min(g.vertices_count()),
              m_ids(g.vertices_count()),
              m_group_id(0) {}
        void search() {
            for (auto v = m_g.crbegin(); v != m_g.crend(); ++v)
                if (m_pre.is_unset(*v)) search(*v);
        }
        void search(const V& v) {
            m_pre.set_next(v);
            size_t min = m_min[v] = m_pre[v];
            m_stack.push(&v);
            for (auto w = v.cbegin(); w != v.cend(); ++w) {
                if (m_pre.is_unset(*w)) search(*w);
                if (m_min[*w] < min) min = m_min[*w];
            }
            if (min < m_min[v])
                m_min[v] = min;
            else {
                const V* w;
                do {
                    w = m_stack.pop();
                    m_ids[*w] = m_group_id;
                    m_min[*w] = m_g.vertices_count();
                } while (v != *w);
                ++m_group_id;
            }
        }
    };
    Searcher s(g);
    s.search();
    return std::move(s.m_ids);
}

template <typename G, typename E>
auto compose_path_tree(const G& g, const E& edges_b, const E& edges_e) {
    AdjacencyLists<GraphType::DIGRAPH, typename G::vertex_type::value_type,
                   typename G::edge_type::value_type>
        mst;
    for (auto v = g.cbegin(); v != g.cend(); ++v) mst.create_vertex(v->value());
    for (auto e = edges_b; e != edges_e; ++e)
        if (e->m_target)
            mst.add_edge(mst[e->source()], mst[e->target()],
                         e->edge().weight());
    return mst;
}

template <typename G>
auto pq_mst(const G& g) {
    using vertex_t = typename G::vertex_type;
    using w_t = typename G::edge_type::value_type;

    struct PqMstSearcher {
        const G& m_g;
        Array<w_t> m_weights;
        Array<typename G::vertex_type::const_edges_iterator::entry_type> m_fr,
            m_mst;
        PqMstSearcher(const G& g, size_t size)
            : m_g(g), m_weights(size), m_fr(size), m_mst(size) {
            for (auto& e : m_fr) e.m_target = nullptr;
            for (auto& e : m_mst) e.m_target = nullptr;
        }
        void search(const vertex_t& v) {
            VertexHeap<const vertex_t*, w_t> heap(m_g.vertices_count(),
                                                  m_weights);
            heap.push(&v);
            while (!heap.empty()) {
                const vertex_t& w = *heap.pop();
                m_mst[w] = m_fr[w];
                for (auto e = w.cedges_begin(); e != w.cedges_end(); ++e) {
                    const vertex_t& t = e->target();
                    w_t weight = e->edge().weight();
                    if (!m_fr[t].m_target) {
                        m_weights[t] = weight;
                        heap.push(&t);
                        m_fr[t] = *e;
                    } else if (!m_mst[t].m_target && weight < m_weights[t]) {
                        m_weights[t] = weight;
                        heap.move_up(&w);
                        m_fr[t] = *e;
                    }
                }
            }
        }
        void search() {
            for (auto v = m_g.cbegin(); v != m_g.cend(); ++v)
                if (!m_mst[*v].m_target) search(*v);
        }
    };

    PqMstSearcher s(g, g.vertices_count());
    s.search();
    return compose_path_tree(g, s.m_mst.cbegin(), s.m_mst.cend());
}

template <typename G>
struct Spt {
    using vertex_t = typename G::vertex_type;
    using edge_t = typename G::vertex_type::const_edges_iterator::entry_type;
    using weight_t = typename G::edge_type::value_type;

    Array<weight_t> m_distance;
    Array<edge_t> m_spt;
    Spt(const G& g, const vertex_t& vertex, weight_t max_weight)
        : m_distance(g.vertices_count(), max_weight),
          m_spt(g.vertices_count()) {
        for (auto& e : m_spt) e.m_target = nullptr;
        VertexHeap<const vertex_t*, weight_t> heap(g.vertices_count(),
                                                   m_distance);
        for (const vertex_t* v = g.cbegin(); v != g.cend(); ++v) heap.push(v);
        m_distance[vertex] = 0;
        heap.move_up(&vertex);
        while (!heap.empty()) {
            const vertex_t* v = heap.pop();
            if (v != &vertex && !m_spt[*v].m_target) return;
            for (auto e = v->cedges_begin(); e != v->cedges_end(); ++e) {
                const vertex_t* w = e->m_target;
                weight_t distance = m_distance[*v] + e->edge().weight();
                if (m_distance[*w] > distance) {
                    m_distance[*w] = distance;
                    heap.move_up(w);
                    m_spt[*w] = *e;
                }
            }
        }
    }
};

template <typename G>
struct FullSpts {
    using vertex_t = typename G::vertex_type;
    using w_t = typename G::edge_type::value_type;
    using edge_t = typename G::vertex_type::const_edges_iterator::entry_type;
    const G& m_g;
    Array<Array<w_t>> m_distances;
    Array<Array<edge_t>> m_spts;
    FullSpts(const G& g, w_t max_weight)
        : m_g(g), m_distances(g.vertices_count()), m_spts(g.vertices_count()) {
        size_t i = 0;
        for (auto v = g.cbegin(); v != g.cend(); ++v, ++i) {
            Spt spt(g, *v, max_weight);
            m_distances[i] = std::move(spt.m_distance);
            m_spts[i] = std::move(spt.m_spt);
        }
    }
    w_t distance(size_t v, size_t w) { return m_distances[v][w]; }
    const edge_t& path(size_t v, size_t w) { return m_spts[w][v]; }
    const edge_t& path_r(size_t v, size_t w) { return m_spts[v][w]; }
    std::pair<const vertex_t*, const vertex_t*> diameter() {
        size_t v_max = 0;
        size_t w_max = 0;
        for (auto v = m_g.cbegin(); v != m_g.cend(); ++v)
            for (auto w = m_g.cbegin(); w != m_g.cend(); ++w)
                if (path(*v, *w).m_target &&
                    distance(*v, *w) > distance(v_max, w_max)) {
                    v_max = *v;
                    w_max = *w;
                }
        return {&m_g[v_max], &m_g[w_max]};
    }
};

template <typename G>
struct DagLpt {
    using w_t = typename G::edge_type::value_type;
    using edge_t = typename G::vertex_type::const_edges_iterator::entry_type;
    Array<w_t> m_distances;
    Array<edge_t> m_lpt;
    DagLpt(const G& g)
        : m_distances(g.vertices_count()), m_lpt(g.vertices_count()) {
        m_distances.fill(0);
        for (auto& e : m_lpt) e.m_target = nullptr;

        TopologicalSorter sorter(g);
        sorter.search();
        auto& t_sorted = sorter.m_post_i;

        for (auto i = t_sorted.crbegin(); i != t_sorted.crend(); ++i) {
            auto& v = g[*i];
            for (auto e = v.cedges_begin(); e != v.cedges_end(); ++e) {
                auto& w = e->target();
                auto distance = m_distances[v] + e->edge().weight();
                if (m_distances[w] < distance) {
                    m_distances[w] = distance;
                    m_lpt[w] = *e;
                }
            }
        }
    }
};

template <typename G>
struct DagFullSpts {
    using vertex_t = typename G::vertex_type;
    using w_t = typename G::edge_type::value_type;
    using edge_t = typename G::vertex_type::const_edges_iterator::entry_type;
    static auto empty_edge_it() {
        auto create = []() {
            edge_t e;
            e.m_target = nullptr;
            return e;
        };
        static edge_t e(create());
        return e;
    }
    const G& m_g;
    Array<Array<w_t>> m_distances;
    Array<Array<edge_t>> m_spts;
    DagFullSpts(const G& g, w_t max_weight)
        : m_g(g),
          m_distances(g.vertices_count(),
                      Array<w_t>(g.vertices_count(), max_weight)),
          m_spts(g.vertices_count(),
                 Array<edge_t>(g.vertices_count(), empty_edge_it())) {
        for (size_t i = 0; i < g.vertices_count(); ++i)
            if (!m_spts[i][i].m_target) dfs(g[i]);
    }
    void dfs(const vertex_t& s) {
        for (auto e = s.cedges_begin(); e != s.cedges_end(); ++e) {
            auto& t = e->target();
            auto weight = e->edge().weight();
            if (m_distances[s][t] > weight) {
                m_distances[s][t] = weight;
                m_spts[s][t] = *e;
            }
            if (!m_spts[t][t].m_target) dfs(t);
            for (auto it = m_g.cbegin(); it != m_g.cend(); ++it) {
                auto& i = *it;
                auto distance = m_distances[t][i] + weight;
                if (m_spts[t][i].m_target && m_distances[s][i] > distance) {
                    m_distances[s][i] = distance;
                    m_spts[s][i] = *e;
                }
            }
        }
    }
};

/**
 * Bellman-Ford algorithm for negative cycles search.
 */
template <typename G>
ArrayCycle find_negative_cycle(const G& g, const typename G::vertex_type& s,
                               typename G::edge_type::value_type sentinel) {
    using vertex_type = typename G::vertex_type;
    using w_t = typename G::edge_type::value_type;
    using edge_t = typename G::vertex_type::const_edges_iterator::entry_type;

    Array<w_t> weights(g.vertices_count(), sentinel);
    Array<edge_t> spt(g.vertices_count());
    for (auto& s : spt) s.m_target = nullptr;

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
            if (!completed) queue.push_back(nullptr);
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

    using cycle_g_type = AdjacencyLists<GraphType::DIGRAPH, int>;
    cycle_g_type gg;
    for (auto v = g.cbegin(); v != g.cend(); ++v) gg.create_vertex(*v);

    for (auto& s : spt)
        if (s.m_target) gg.add_edge(gg[s.source()], gg[s.target()]);

    struct Searcher
        : public Post_dfs_base<cycle_g_type, size_t, size_t, Searcher> {
        using Base = Post_dfs_base<cycle_g_type, size_t, size_t, Searcher>;
        bool m_found;
        Array<size_t> m_cycle;
        Searcher(const cycle_g_type& g, w_t sentinel)
            : Base(g), m_found(false), m_cycle(g.vertices_count(), sentinel) {}
        void search_vertex(const typename Base::vertex_type& v) {
            if (!m_found) Base::search_vertex(v);
        }
        void visit_edge(const typename Base::edge_type& e) {
            auto& v = e.source();
            auto& w = e.target();
            if (!m_found && Base::m_pre[w] < Base::m_pre[v] &&
                Base::m_post.is_unset(w))
                m_found = true;
            if (m_found) m_cycle[v] = w;
        }
    };
    Searcher searcher(gg, sentinel);
    searcher.search();
    if (searcher.m_found)
        return {std::move(searcher.m_cycle), static_cast<size_t>(sentinel)};
    return {{}, static_cast<size_t>(sentinel)};
}

}  // namespace Graph
