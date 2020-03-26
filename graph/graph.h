#pragma once

#include <map>
#include <iostream>

#include "dfs.h"
#include "array.h"
#include "stack.h"
#include "string_utils.h"
#include "two_dimensional_array.h"
#include "adjacency_lists.h"

namespace Graph {

    template<typename G, typename T = typename G::vertex_type::value_type>
        class Constructor {
            private:
                using vertex_type = typename G::vertex_type;
                G& graph_;
                std::map<T, typename G::vertex_type&> vertices_;
            public:
                Constructor(G& graph) :graph_(graph) {}
                Constructor& add_edge(const T& l1, const T& l2) {
                    auto& v1 = get_or_create_vertex(l1);
                    auto& v2 = get_or_create_vertex(l2);
                    graph_.add_edge(v1, v2);
                    return *this;
                }
                vertex_type& get_or_create_vertex(const T& l) {
                    auto it = vertices_.find(l);
                    if (it == vertices_.end()) {
                        auto& v = graph_.create_vertex(l);
                        vertices_.insert({l, v});
                        return v;
                    } else
                        return it->second;
                }
                vertex_type& get_vertex(const T& label) {
                    auto it = vertices_.find(label);
                    if (it == vertices_.end())
                        throw std::runtime_error("vertex "_str + label + " not found");
                    return it->second;
                };
        };

    template<typename G>
        class Builder {
            private:
                class Vertex;
                friend class Vertex;
                using vertex_type = typename G::vertex_type;
                using value_type = typename vertex_type::value_type;

                G g_;
                std::map<value_type, vertex_type* const> map_;

                vertex_type* const get_or_create_vertex(const value_type& value) {
                    auto vertex = map_.find(value);
                    if (vertex == map_.end()) {
                        auto p = &g_.create_vertex(value);
                        map_.insert({value, p});
                        return p;
                    } else
                        return vertex->second;
                }
                template<typename... Args>
                    void add_edge(vertex_type* const v, const value_type& w, Args&&... args) {
                        g_.add_edge(*v, *get_or_create_vertex(w), std::forward<Args>(args)...);
                    }
                template<typename E, typename... Es>
                    void add_edges(Vertex& vertex, E&& edge, Es&&... edges) {
                        g_.add_edge(*vertex.vertex_, *get_or_create_vertex(std::forward<E>(edge)));
                        add_edges(vertex, std::forward<Es>(edges)...);
                    }
                void add_edges(Vertex&) {}
            public:
                Vertex for_vertex(const value_type& value) {
                    return Vertex(*this, get_or_create_vertex(value));
                }
                G build() { return std::move(g_); }
        };

    template<typename G>
        class Builder<G>::Vertex {
            private:
                using builder_type = Builder<G>;
                using vertex_type = typename G::vertex_type;
                using value_type = typename vertex_type::value_type;

                builder_type& builder_;
            public:
                vertex_type* const vertex_;
                Vertex(builder_type& b, vertex_type* const v) :builder_(b), vertex_(v) {}

                template<typename... Args>
                    Vertex& add_edge(const value_type& v, Args&&... args) {
                        builder_.add_edge(vertex_, v, std::forward<Args>(args)...);
                        return *this;
                    }
                template<typename... E>
                    Vertex& add_edges(E&&... edges) {
                        builder_.add_edges(*this, std::forward<E>(edges)...);
                        return *this;
                    }
                Vertex for_vertex(const value_type& v) { return builder_.for_vertex(v); }
                G build() { return builder_.build(); }
        };

    template<typename It, typename F>
        void print_collection(const It& b, const It& e, const char* del, F f, std::ostream& stream) {
            if (b == e)
                return;
            auto item = b;
            stream << f(*item);
            for (++item; item != e; ++item)
                stream << del << f(*item);
        }

    template<typename It>
        void print_collection(const It& b, const It& e, const char* del, std::ostream& stream) {
            print_collection(b, e, del, [](auto& v) { return v; }, stream);
        }

    template<typename T>
        class Fixed_size_stack {
            private:
                Array<T> array_;
                size_t index_; 
            public:
                Fixed_size_stack(size_t size) :array_(size), index_(0) {}
                void push(const T& t) {
                    array_[index_++] = t;
                }
                Array<T> to_reversed_array() {
                    for (size_t i = 0; i < index_ / 2; ++i)
                        std::swap(array_[i], array_[index_ - i - 1]);
                    return std::move(array_);
                }
        };

    template<typename V>
        auto count_vertex_edges(const V& v) {
            size_t count = 0;
            for (auto e = v.cbegin(); e != v.cend(); ++e)
                ++count;
            return count;
        }

    template<typename G, typename V = typename G::vertex_type>
        bool has_simple_path(const G& graph, const V& v1, const V& v2) {
            Array<bool> visited(graph.vertices_count());
            for (auto& b : visited)
                b = false;
            static struct {
                bool has_simple_path(const G& graph, const V& v1, const V& v2, Array<bool>& visited) {
                    if (v1 == v2)
                        return true;
                    visited[v1] = true;
                    for (auto v = v1.cbegin(); v != v1.cend(); ++v)
                        if (!visited[*v])
                            if (has_simple_path(graph, *v, v2, visited))
                                return true;
                    return false;
                }
            } helper;
            return helper.has_simple_path(graph, v1, v2, visited);
        }

    template<typename G, typename V = typename G::vertex_type>
        Array<const V*> compose_hamilton_path(const G& graph, const V& s, const V& t) {
            if (graph.vertices_count() < 1)
                return {};
            if (graph.vertices_count() < 2)
                return {&graph[0]};
            struct Helper {
                Array<bool> visited_;
                Fixed_size_stack<const V*> stack_;
                const size_t size_;
                Helper(size_t size) :visited_(size, false), stack_(size), size_(size) {}
                bool compose_path(const V& s, const V& t) {
                    return compose_path(s, t, size_ - 1);
                }
                bool compose_path(const V& s, const V& t, size_t depth) {
                    bool has;
                    if (s == t)
                        has = depth == 0;
                    else {
                        has = false;
                        visited_[s] = true;
                        for (auto v = s.cbegin(); v != s.cend() && !has; ++v)
                            has = !visited_[*v] && compose_path(*v, t, depth - 1);
                        if (!has)
                            visited_[s] = false;
                    }
                    if (has)
                        stack_.push(&s);
                    return has;
                }
            };
            Array<const V*> path;
            Helper helper(graph.vertices_count());
            if (helper.compose_path(s, t))
                path = std::move(helper.stack_.to_reversed_array());
            return path;
        }

    template<typename G, typename V = typename G::vertex_type>
        auto compose_euler_tour(const G& g, const V& s) {
            using Path = Forward_list<const V*>;
            Path path;
            for (auto w = g.cbegin(); w != g.cend(); ++w)
                if (count_vertex_edges(*w) % 2 != 0)
                    return path;

            struct Helper {
                const G& original_g_;
                G& g_;
                Stack<V*> stack_;
                Path& path_;
                Helper(const G& original_graph, G& g, Path& path) 
                    :original_g_(original_graph), g_(g), path_(path)
                {}
                V* tour(V* v) {
                    while (true) {
                        auto it = v->begin();
                        if (it == v->end())
                            break;
                        stack_.push(v);
                        auto& w = *it;
                        g_.remove_edge(*v, w);
                        v = &w;
                    }
                    return v;
                }
                void compose(V* v) {
                    push_original_v(v);
                    while (tour(v) == v && !stack_.empty()) {
                        v = stack_.pop();
                        push_original_v(v);
                    }
                }
                void push_original_v(const V* v) { 
                    path_.push_back(&original_g_[*v]);
                }
            };
            auto g_copy = g;
            Helper(g, g_copy, path).compose(&g_copy[s]);
            return path;
        }

    template<typename G, typename V = typename G::vertex_type>
        Forward_list<std::pair<const V*, const V*>> find_bridges(const G& graph) {
            struct Searcher {
                const G& g_;
                Array<int> orders_;
                Array<int> mins_;
                int order_;
                Forward_list<std::pair<const V*, const V*>> bridges_;
                Searcher(const G& g) :g_(g), orders_(g.vertices_count()), mins_(g.vertices_count()), order_(-1) {
                    for (auto& o : orders_) o = -1;
                }
                void search() {
                    for (auto v = g_.cbegin(); v != g_.cend(); ++v)
                        if (orders_[*v] == -1)
                            search(*v, *v);
                }
                void search(const V& v, const V& w) {
                    orders_[w] = ++order_;
                    mins_[w] = orders_[w];
                    for (auto t = w.cbegin(); t != w.cend(); ++t)
                        if (orders_[*t] == -1) {
                            search(w, *t);
                            if (mins_[w] > mins_[*t]) mins_[w] = mins_[*t];
                            if (mins_[*t] == orders_[*t])
                                bridges_.emplace_back(&w, &*t);
                        } else if (v != *t)
                            mins_[w] = mins_[*t];
                }
            };
            Searcher s(graph);
            s.search();
            return std::move(s.bridges_);
        }

    template <typename V>
        class Shortest_paths_matrix {
            public:
                using parents_type = Two_dimensional_array<const V*>;
            private:
                parents_type parents_;
            public:
                Shortest_paths_matrix(parents_type&& parents) :parents_(parents) {}
                auto find_path(const V& v, const V& w) {
                    Forward_list<const V*> path;
                    auto row = parents_[w];
                    for (auto t = &v; t != &w; t = row[*t]) path.push_back(t);
                    path.push_back(&w);
                    return path;
                }
        };

    template<typename G, typename V = typename G::vertex_type>
        Shortest_paths_matrix<V> find_shortest_paths(const G& g) {
            typename Shortest_paths_matrix<V>::parents_type all_parents(g.vertices_count(), g.vertices_count());
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
                            if (!parents[*t])
                                queue.emplace_back(e.second, *t);
                    }
                }
            }
            return {std::move(all_parents)};
        }

    template<typename G>
        G warshall_transitive_closure(const G& g) {
            G g_copy = g;
            for (auto& v : g_copy)
                g_copy.add_edge(v, v);
            for (auto& i : g_copy)
                for (auto& s : g_copy)
                    if (g_copy.has_edge(s, i))
                        for (auto& t : g_copy)
                            if (g_copy.has_edge(i, t))
                                g_copy.add_edge(s, t);
            return g_copy;
        }

    template<typename G, typename V = typename G::vertex_type>
        G dfs_transitive_closure(const G& g) {
            struct Helper {
                G& g_;
                Array<bool> a_;
                Helper(G& g) :g_(g), a_(g.vertices_count()) {}
                void search() {
                    for (auto& v : g_) {
                        a_.fill(false);
                        search(v, v, v);
                    }
                }
                void search(V& vv, V& v, V& w) {
                    if (!a_[w]) {
                        a_[w] = true;
                        if (!g_.has_edge(vv, w))
                            g_.add_edge(vv, w);
                        for (auto& t : w)
                            search(vv, w, t);
                    }
                }
            };
            auto g_copy = g;
            Helper(g_copy).search();
            return g_copy;
        }

    template<typename G, typename D>
        class Dfs_tracer_base : public Post_dfs_base<G, size_t, size_t, D> {
            private:
                int depth_;
                Stack<std::string> lines_stack_;
            protected:
                std::ostream& stream_;
                using Base = Post_dfs_base<G, size_t, size_t, D>;
                using vertex_type = typename Base::vertex_type;
                using edge_type = typename Base::edge_type;
                size_t last_root_id_;
            public:
                Dfs_tracer_base(const G& g, std::ostream& stream) 
                    :Base(g), depth_(-1), stream_(stream), last_root_id_(-1) 
                {}
                void print_stack() {
                    while (!lines_stack_.empty())
                        stream_ << lines_stack_.pop() << std::endl;
                }
                void search() {
                    Base::search();
                    stream_ << last_root_id_ << std::endl;
                    print_stack();
                }
                void visit_vertex(const vertex_type& v) {
                    ++depth_;
                    if (depth_ == 0) {
                        if (lines_stack_.empty()) {
                            stream_ << v << std::endl;
                        } else
                            print_stack();
                        last_root_id_ = v;
                    }
                }
                void visit_edge(const edge_type& e) {
                    auto& v = e.source();
                    auto& w = e.target();
                    std::string str;
                    if (Base::pre_[w] > Base::pre_[v])
                        str = "down";
                    else if (Base::post_.is_unset(w))
                        str = "back";
                    else
                        str = "cross";
                    lines_stack_.push(compose_stack_line(str, e));
                }
                void search_post_process(const vertex_type& v) {
                    --depth_;
                    Base::search_post_process(v);
                }
                template<typename E, typename ET = typename E::edge_type::value_type>
                    struct Edge_appender {
                        static void append(std::ostream& stream, const E& e) {
                            stream << " (" << e.edge().weight() << ")";
                        }
                    };
                template<typename E>
                    struct Edge_appender<E, bool> {
                        static void append(std::ostream& stream, const E& e) {}
                    };
                std::string compose_stack_line(const std::string label, const edge_type& e) {
                    std::stringstream ss;
                    for (int i = 0; i < depth_; ++i) ss << " ";
                    ss << " " << e.target();
                    Edge_appender<edge_type>::append(ss, e);
                    ss << " (" << label << ")";
                    return ss.str();
                }

        };

    template<typename G>
        void trace_dfs(const G& g, std::ostream& stream) {
            class Tracer : public Dfs_tracer_base<G, Tracer> {
                public:
                    Tracer(const G& g, std::ostream& stream) :Dfs_tracer_base<G, Tracer>(g, stream) {}
            };
            Tracer(g, stream).search();
        }

    template<typename G, typename D>
        struct Inverter_base {
            G invert(const G& g) {
                G inverted;
                for (auto v = g.cbegin(); v != g.cend(); ++v)
                    inverted.create_vertex(v->value());
                static_cast<D*>(this)->do_invert(g, inverted);
                return inverted;
            }
        };

    template<typename G, typename E = typename G::edge_type::value_type>
        struct Inverter : public Inverter_base<G, Inverter<G, E>> {
            void do_invert(const G& g, G& inverted) {
                for (auto v = g.cbegin(); v != g.cend(); ++v)
                    for (auto e = v->cedges_begin(); e != v->cedges_end(); ++e)
                        inverted.add_edge(inverted[e->target()], inverted[*v],
                                          e->edge().weight());
            }
        };

    template<typename G>
        struct Inverter<G, bool> : public Inverter_base<G, Inverter<G, bool>> {
            void do_invert(const G& g, G& inverted) {
                for (auto v = g.cbegin(); v != g.cend(); ++v)
                    for (auto w = v->cbegin(); w != v->cend(); ++w)
                        inverted.add_edge(inverted[*w], inverted[*v]);
            }
        };

    template<typename G>
        G invert(const G& g) {
            static Inverter<G> inverter;
            return inverter.invert(g);
        }

    template<typename G>
        Array<size_t> topological_sort_rearrange(const G& g) {
            auto inverted = invert(g);
            Post_dfs<G, size_t, size_t> d(inverted);
            d.search();
            return d.post_.to_array();
        }

    template<typename G>
        struct Topological_sorter : public Post_dfs_base<G, size_t, size_t, Topological_sorter<G>> {
            using Base = Post_dfs_base<G, size_t, size_t, Topological_sorter<G>>;
            Array<size_t> post_i_;
            Topological_sorter(const G& g) :Base(g), post_i_(g.vertices_count()) {}
            void search_post_process(const typename Base::vertex_type& v) {
                Base::search_post_process(v);
                post_i_[Base::post_[v]] = v;
            }
        };

    template<typename G>
        Array<size_t> topological_sort_relabel(const G& g) {
            auto inverted = invert(g);
            Topological_sorter s(inverted);
            s.search();
            return std::move(s.post_i_);
        }

    template<typename G>
        void trace_dfs_topo_sorted(const G& g, std::ostream& stream) {
            class Tracer : public Dfs_tracer_base<G, Tracer> {
                public:
                    using Base = Dfs_tracer_base<G, Tracer>;
                    using vertex_type = typename Base::vertex_type;
                    Tracer(const G& g, std::ostream& stream) :Base(g, stream) {}
                    void search() {
                        auto t_order = topological_sort_relabel(Base::g_);
                        for (auto it = Base::g_.crbegin(); it != Base::g_.crend(); ++it) {
                            const vertex_type& v = Base::g_[t_order[*it]];
                            if (Base::pre_.is_unset(v))
                                Base::search_vertex(v);
                        }
                        Base::stream_ << Base::last_root_id_ << std::endl;
                        Base::print_stack();
                    }
            };
            Tracer(g, stream).search();
        }

    template<typename G>
        void trace_bfs(const G& g) {
            using V = typename G::vertex_type;
            Counters<bool> pre(g.vertices_count());
            for (auto vv = g.cbegin(); vv != g.cend(); ++vv) {
                if (pre.is_unset(*vv)) {
                    struct Entry {
                        const V* const v_;
                        const int d_;
                        Entry(const V* const v, int d) :v_(v), d_(d) {}
                    };
                    Forward_list<Entry> queue;
                    queue.emplace_back(&*vv, 0);
                    while (!queue.empty()) {
                        auto p = queue.pop_front();
                        auto& w = *p.v_;
                        for (auto t = w.cbegin(); t != w.cend(); ++t) {
                            if (pre.is_unset(*t)) {
                                pre.set_next(*t);
                                queue.emplace_back(&*t, p.d_ + 1);
                            }
                            if (w != *t) {
                                for (int i = 0; i < p.d_; ++i)
                                    std::cout << " ";
                                std::cout << w << " " << *t << std::endl;
                            }
                        }
                    }
                }
            }
        }

    class Invalid_dag_exception : public std::runtime_error {
        public:
            Invalid_dag_exception() :std::runtime_error("invalid dag") {}
    };

    template<typename G>
        void validate_dag(const G& g) {
            struct Searcher : public Post_dfs_base<G, size_t, size_t, Searcher> {
                using Base = Post_dfs_base<G, size_t, size_t, Searcher>;
                Searcher(const G& g) :Base(g) {}
                void visit_edge(const typename Base::edge_type& e) {
                    auto& v = e.source();
                    auto& w = e.target();
                    if (Base::pre_[w] < Base::pre_[v] && Base::post_.is_unset(w))
                        throw Invalid_dag_exception(); // back link found
                }
            };
            Searcher(g).search();
        }

    template<typename G>
        bool is_dag(const G& g) {
            try {
                validate_dag(g);
                return true;
            } catch (const Invalid_dag_exception&) {
                return false;
            }
        }

    template<typename G, typename V = typename G::vertex_type>
        auto topological_sort_sinks_queue(const G& g) {
            Array<size_t> in(g.vertices_count());
            in.fill(0);
            for (auto v = g.cbegin(); v != g.cend(); ++v)
                for (auto w = v->cbegin(); w != v->cend(); ++w)
                    ++in[*w];
            Forward_list<const V* const> queue;
            for (auto v = g.cbegin(); v != g.cend(); ++v)
                if (in[*v] == 0)
                    queue.push_back(&*v);
            Array<size_t> ordered(g.vertices_count());
            for (auto ordered_it = ordered.begin(); !queue.empty(); ++ordered_it) {
                auto v = queue.pop_front();
                *ordered_it = *v;
                for (auto w = v->cbegin(); w != v->cend(); ++w)
                    if (--in[*w] == 0)
                        queue.push_back(&*w);
            }
            return ordered;
        }

    template<typename G, typename V = typename G::vertex_type>
        Array<size_t> strong_components_kosaraju(const G& g) {
            struct Searcher {
                const G& g_;
                Array<size_t> ids_;
                size_t group_id_;
                Searcher(const G& g) 
                    :g_(g), ids_(g.vertices_count()), group_id_(-1) 
                { 
                    ids_.fill(-1);
                }
                void search() {
                    auto t_order = topological_sort_relabel(g_);
                    for (auto v = g_.crbegin(); v != g_.crend(); ++v) {
                        auto& t = g_[t_order[*v]];
                        if (ids_[t] == static_cast<size_t>(-1)) {
                            ++group_id_;
                            search(t);
                        }
                    }
                }
                void search(const V& v) {
                    ids_[v] = group_id_;
                    for (auto w = v.cbegin(); w != v.cend(); ++w)
                        if (ids_[*w] == static_cast<size_t>(-1))
                            search(*w);
                }
            };

            Searcher s(g);
            s.search();
            return std::move(s.ids_);
        }

    template<typename G>
        Array<size_t> strong_components_tarjan(const G& g) {
            using V = typename G::vertex_type;
            struct Searcher {
                const G& g_;
                Counters<size_t> pre_;
                Array<size_t> min_;
                Array<size_t> ids_;
                size_t group_id_;
                Stack<const V* const> stack_;
                Searcher(const G& g) 
                    :g_(g), pre_(g.vertices_count()), min_(g.vertices_count()), ids_(g.vertices_count()), group_id_(0) 
                {}
                void search() {
                    for (auto v = g_.crbegin(); v != g_.crend(); ++v)
                        if (pre_.is_unset(*v))
                            search(*v);
                }
                void search(const V& v) {
                    pre_.set_next(v);
                    size_t min = min_[v] = pre_[v];
                    stack_.push(&v);
                    for (auto w = v.cbegin(); w != v.cend(); ++w) {
                        if (pre_.is_unset(*w))
                            search(*w);
                        if (min_[*w] < min)
                            min = min_[*w];
                    }
                    if (min < min_[v])
                        min_[v] = min;
                    else {
                        const V* w;
                        do {
                            w = stack_.pop();
                            ids_[*w] = group_id_;
                            min_[*w] = g_.vertices_count();
                        } while (v != *w);
                        ++group_id_;
                    }
                }
            };
            Searcher s(g);
            s.search();
            return std::move(s.ids_);
        }

    template<typename G, typename E>
        auto compose_path_tree(const G& g, const E& edges_b, const E& edges_e) {
            Adjacency_lists<Graph_type::DIGRAPH, typename G::vertex_type::value_type, typename G::edge_type::value_type> mst;
            for (auto v = g.cbegin(); v != g.cend(); ++v)
                mst.create_vertex(v->value());
            for (auto e = edges_b; e != edges_e; ++e)
                if (e->target_)
                    mst.add_edge(mst[e->source()], mst[e->target()], e->edge().weight());
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
        struct Spt {
            using vertex_t = typename G::vertex_type;
            using edge_t = typename G::vertex_type::const_edges_iterator::entry_type;
            using weight_t = typename G::edge_type::value_type;

            Array<weight_t> distances_;
            Array<edge_t> spt_;
            Spt(const G& g, const vertex_t& vertex, weight_t max_weight) 
                :distances_(g.vertices_count(), max_weight), spt_(g.vertices_count()) 
            {
                for (auto& e : spt_) e.target_ = nullptr;
                Vertex_heap<const vertex_t*, weight_t> heap(g.vertices_count(), distances_);
                for (const vertex_t* v = g.cbegin(); v != g.cend(); ++v)
                    heap.push(v);
                distances_[vertex] = 0;
                heap.move_up(&vertex);
                while (!heap.empty()) {
                    const vertex_t* v = heap.pop();
                    if (v != &vertex && !spt_[*v].target_)
                        return;
                    for (auto e = v->cedges_begin(); e != v->cedges_end(); ++e) {
                        const vertex_t* w = e->target_;
                        weight_t distance = distances_[*v] + e->edge().weight();
                        if (distances_[*w] > distance) {
                            distances_[*w] = distance;
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
            Array<Array<w_t>> distances_;
            Array<Array<edge_t>> spts_;
            Full_spts(const G& g, w_t max_weight) 
                :g_(g), distances_(g.vertices_count()), spts_(g.vertices_count()) 
            {
                size_t i = 0;
                for (auto v = g.cbegin(); v != g.cend(); ++v, ++i) {
                    Spt spt(g, *v, max_weight);
                    distances_[i] = std::move(spt.distances_);
                    spts_[i] = std::move(spt.spt_);
                }
            }
            w_t distance(size_t v, size_t w) { return distances_[v][w]; }
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
                return {&g_[v_max], &g_[w_max]};
            }
        };

    template<typename G>
        struct Dag_lpt {
            using w_t = typename G::edge_type::value_type;
            using edge_t = typename G::vertex_type::const_edges_iterator::entry_type;
            Array<w_t> distances_;
            Array<edge_t> lpt_;
            Dag_lpt(const G& g) :distances_(g.vertices_count()), lpt_(g.vertices_count()) {
                distances_.fill(0);
                for (auto& e : lpt_) e.target_ = nullptr;

                Topological_sorter sorter(g);
                sorter.search();
                auto& t_sorted = sorter.post_i_;

                for (auto i = t_sorted.crbegin(); i != t_sorted.crend(); ++i) {
                    auto& v = g[*i];
                    for (auto e = v.cedges_begin(); e != v.cedges_end(); ++e) {
                        auto& w = e->target();
                        auto distance = distances_[v] + e->edge().weight();
                        if (distances_[w] < distance) {
                            distances_[w] = distance;
                            lpt_[w] = *e;
                        }
                    }
                }
            }
        };

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

}

