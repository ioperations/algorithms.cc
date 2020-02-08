#pragma once

#include "dfs.h"
#include "array.h"
#include "stack.h"
#include "string_utils.h"
#include "two_dimensional_array.h"

#include <map>
#include <iostream>

namespace Graph {

    enum class Graph_type {
        GRAPH, DIGRAPH
    };

    template<typename G, typename T = typename G::value_type>
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
                using value_type = typename G::value_type;
                using vertex_type = typename G::vertex_type;
                using edge_value_type = typename G::edge_type::value_type;

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
                void add_edge(vertex_type* const v, const value_type& w, const edge_value_type& e) {
                    g_.add_edge(*v, *get_or_create_vertex(w), e);
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
                using value_type = typename G::value_type;
                using edge_value_type = typename G::edge_type::value_type;
                builder_type& builder_;
            public:
                vertex_type* const vertex_;
                Vertex(builder_type& b, vertex_type* const v) :builder_(b), vertex_(v) {}

                Vertex& add_edge(const value_type& v, const edge_value_type& e) {
                    builder_.add_edge(vertex_, v, e);
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

    template<typename T>
        class Vertex_base {
            protected:
                T value_;
                Vertex_base(const T& value) :value_(value) {}
                Vertex_base() = default;
            public:
                const T& value() const { return value_; }
                void set_value(const T& value) { value_ = value; }
                bool operator==(const Vertex_base& o) const {
                    return this == &o;
                }
                bool operator!=(const Vertex_base& o) const {
                    return !operator==(o);
                }
                friend std::ostream& operator<<(std::ostream& stream, const Vertex_base& v) {
                    return stream << v.value_;
                }
        };

    template<typename V, bool T_is_const>
        class Edges_iterator_entry_base {
            protected:
                using vertex_type = std::conditional_t<T_is_const, const V, V>;
            public:
                vertex_type* source_;
                vertex_type* target_;
                Edges_iterator_entry_base() = default; 
                Edges_iterator_entry_base(vertex_type* source) :source_(source) {}
                vertex_type& source() const { return *source_; }
                vertex_type& target() const { return *target_; }
        };

    template<typename V, typename E, bool T_is_const, typename ET = typename E::value_type>
        class Edges_iterator_entry : public Edges_iterator_entry_base<V, T_is_const> {
            private:
                using Base = Edges_iterator_entry_base<V, T_is_const>;
                using vertex_type = typename Base::vertex_type;
                using edge_type = std::conditional_t<T_is_const, const E, E>;
            public:
                edge_type* edge_;
                Edges_iterator_entry() = default;
                Edges_iterator_entry(vertex_type* source) :Base(source) {}
                edge_type& edge() const { return *edge_; }
        };

    template<typename V, typename E, bool T_is_const>
        class Edges_iterator_entry<V, E, T_is_const, bool> : public Edges_iterator_entry_base<V, T_is_const> {
            private:
                using Base = Edges_iterator_entry_base<V, T_is_const>;
                using vertex_type = typename Base::vertex_type;
            public:
                Edges_iterator_entry() = default;
                Edges_iterator_entry(vertex_type* source) :Base(source) {}
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
        Array<const V*> compose_hamilton_path(const G& graph) {
            if (graph.vertices_count() < 1)
                return {};
            if (graph.vertices_count() < 2)
                return {&graph.vertex_at(0)};

            Array<bool> visited(graph.vertices_count());
            for (auto& b : visited)
                b = false;
            using Stack = Fixed_size_stack<const V*>;
            static struct {
                bool has_hamilton_path(const G& graph, const V& v1, const V& v2,
                                       Array<bool>& visited, size_t depth, Stack& stack) {
                    bool has;
                    if (v1 == v2)
                        has = depth == 0;
                    else {
                        has = false;
                        visited[v1] = true;
                        for (auto v = v1.cbegin(); v != v1.cend() && !has; ++v)
                            has = !visited[*v] 
                                && has_hamilton_path(graph, *v, v2, visited, depth - 1, stack);
                        if (!has)
                            visited[v1] = false;
                    }
                    if (has)
                        stack.push(&v1);
                    return has;
                }
            } helper;
            Stack stack(graph.vertices_count());
            Array<const V*> path;
            if (helper.has_hamilton_path(graph, graph.vertex_at(0), graph.vertex_at(1), 
                                         visited, graph.vertices_count() - 1, stack))
                path = std::move(stack.to_reversed_array());
            return path;
        }

    template<typename G, typename V = typename G::vertex_type>
        auto compose_euler_tour(const G& g, const V& v) {
            Array<size_t> degrees(g.vertices_count());
            {
                auto e = degrees.begin();
                for (auto v = g.cbegin(); v != g.cend(); ++v, ++e)
                    *e = count_vertex_edges(*v);
            }
            auto degree = degrees[v] + degrees[v];

            using Path = Forward_list<const V*>;
            Path path;

            if (degree % 2 == 0) {
                bool found = true;
                for (auto w = g.cbegin(); w != g.cend() && found; ++w)
                    if (*w != v && degrees[*w] % 2 != 0)
                        found = false;
                if (found) {
                    using Stack = Stack<V*>;
                    struct Helper {
                        const G& original_g_;
                        G& g_;
                        Stack stack_;
                        Helper(const G& original_graph, G& g) :original_g_(original_graph), g_(g) {}
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
                        void compose(V* v, Path& path) {
                            auto push_original_v = [this, &path](const V* v) {
                                auto& original_v = original_g_.vertex_at(*v);
                                path.push_back(&original_v);
                            };
                            push_original_v(v);
                            while (tour(v) == v && !stack_.empty()) {
                                v = stack_.pop();
                                push_original_v(v);
                            }
                        }
                    };
                    auto g_copy = g;
                    Helper(g, g_copy).compose(&g_copy.vertex_at(v), path);
                }
            }
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

    template<typename G>
    class Dfs_tracer_2 : public Weighted_post_dfs<G, size_t, size_t, Dfs_tracer_2<G>> {
        private:
            using Base = Weighted_post_dfs<G, size_t, size_t, Dfs_tracer_2<G>>;
            using vertex_type = typename Base::vertex_type;
            using edge_type = typename Base::edge_type;

            int depth_;
            Stack<std::string> lines_stack_;
            size_t last_root_id_;
        public:
            Dfs_tracer_2(const G& g) :Base(g), depth_(-1), last_root_id_(-1) {}
            void print_stack() {
                while (!lines_stack_.empty())
                    std::cout << lines_stack_.pop() << std::endl;
            }
            void search() {
                Base::search();
                std::cout << last_root_id_ << std::endl;
                print_stack();
            }
            void visit_vertex(const vertex_type& v) {
                ++depth_;
                if (depth_ == 0) {
                    if (lines_stack_.empty()) {
                        std::cout << v << std::endl;
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
                lines_stack_.push(compose_stack_line(str, v, w));
            }
            void search_post_process(const vertex_type& v) {
                --depth_;
                Base::search_post_process(v);
            }
            std::string compose_stack_line(const std::string label, const vertex_type& v, const vertex_type& w) {
                std::stringstream ss;
                for (int i = 0; i < depth_; ++i) ss << " ";
                ss << " " << w << " (" << label << ")";
                return ss.str();
            }
    };

    template<typename G>
        struct Dfs_tracer : public Dfs::Post_dfs<G> {
            using Base = Dfs::Post_dfs<G>;
            using V = typename G::vertex_type;
            int depth_;
            Stack<std::string> lines_stack_;
            size_t last_root_id_;
            Dfs_tracer(const G& g) 
                :Base(g), depth_(-1), last_root_id_(-1)
            {}
            void print_stack() {
                while (!lines_stack_.empty())
                    std::cout << lines_stack_.pop() << std::endl;
            }
            void visit_vertex(const V& v) {
                ++depth_;
                if (depth_ == 0) {
                    if (lines_stack_.empty()) {
                        std::cout << v << std::endl;
                    } else
                        print_stack();
                    last_root_id_ = v;
                }
            }
            void visit_edge(const V& v, const V& w) {
                std::string str;
                if (Base::pre_[w] > Base::pre_[v])
                    str = "down";
                else if (Base::post_.is_unset(w))
                    str = "back";
                else
                    str = "cross";
                lines_stack_.push(compose_stack_line(str, v, w));
            }
            void search_post_process(const V& v) {
                --depth_;
                Base::search_post_process(v);
            }
            std::string compose_stack_line(const std::string label, const V& v, const V& w) {
                std::stringstream ss;
                for (int i = 0; i < depth_; ++i) ss << " ";
                ss << " " << w << " (" << label << ")";
                return ss.str();
            }
        };

    template<typename G>
        void trace_dfs_2(const G& g) {
            Dfs_tracer_2(g).search();
        }

    template<typename G, typename V = typename G::vertex_type>
        void trace_dfs(const G& g) {
            using namespace Dfs;
            struct Tracer : public Dfs_searcher<G, Dfs_tracer<G>> {
                using Base = Dfs_searcher<G, Dfs_tracer<G>>;
                Tracer(const G& g) :Base(g) {}
                void search_pairs() {
                    Base::search_pairs();
                    std::cout << Base::last_root_id_ << std::endl;
                    Base::print_stack();
                }
            };
            Tracer t(g);
            t.search_pairs();
        }

    template<typename G, typename V = typename G::vertex_type>
        G invert(const G& g) {
            G inverted;
            for (auto v = g.cbegin(); v != g.cend(); ++v)
                inverted.create_vertex(v->value());
            for (auto v = g.cbegin(); v != g.cend(); ++v)
                for (auto w = v->cbegin(); w != v->cend(); ++w)
                    inverted.add_edge(inverted.vertex_at(*w), inverted.vertex_at(*v));
            return inverted;
        }

    template<typename G>
        Array<size_t> topological_sort_rearrange(const G& g) {
            using namespace Dfs;
            auto inverted = invert(g);
            Dfs_searcher<G, Post_dfs<G, bool>> s(inverted);
            s.search_pairs();
            return s.post_.to_array();
        }

    template<typename G>
        Array<size_t> topological_sort_relabel(const G& g) {
            using V = typename G::vertex_type;
            using namespace Dfs;
            struct Searcher : public Post_dfs<G, bool> {
                using Base = Post_dfs<G, bool>;
                Array<size_t> post_i_;
                Searcher(const G& g) :Base(g), post_i_(g.vertices_count()) {}
                void search_post_process(const V& v) {
                    Base::search_post_process(v);
                    post_i_[Base::post_[v]] = v;
                }
            };
            auto inverted = invert(g);
            Dfs_searcher<G, Searcher> s(inverted);
            s.search_pairs();
            return std::move(s.post_i_);
        }
    
    template<typename G>
        void trace_dfs_topo_sorted_2(const G& g) {
            Dfs_tracer_2(g).search();
        }

    template<typename G, typename V = typename G::vertex_type>
        void trace_dfs_topo_sorted(const G& g) {
            using namespace Dfs;
            struct Tracer : public Dfs_searcher<G, Dfs_tracer<G>> {
                using Base = Dfs_searcher<G, Dfs_tracer<G>>;
                Tracer(const G& g) :Base(g) {}
                void search_pairs() {
                    auto t_order = topological_sort_relabel(Base::g_);
                    for (auto it = Base::g_.crbegin(); it != Base::g_.crend(); ++it) {
                        auto& v = Base::g_.vertex_at(t_order[*it]);

                        if (Base::pre_.is_unset(v))
                            Base::search_pairs(v, v);
                    }
                    std::cout << Base::last_root_id_ << std::endl;
                    Base::print_stack();
                }
            };
            Tracer t(g);
            t.search_pairs();
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

    template<typename G, typename V = typename G::vertex_type>
        bool is_dag(const G& g) {
            class Invalid_dag_exception {};
            using namespace Dfs;
            struct Searcher : public Post_dfs<G> {
                using Base = Post_dfs<G>;
                Searcher(const G& g) :Base(g) {}
                void visit_edge(const V& v, const V& w) {
                    if (Base::pre_[w] < Base::pre_[v] && Base::post_.is_unset(w))
                        throw Invalid_dag_exception(); // back link found
                }
            };
            try {
                Dfs_searcher<G, Searcher>(g).search_pairs();
                return true;
            } catch (const Invalid_dag_exception&) {
                return false;
            }
        }

    template<typename G, typename V = typename G::vertex_type>
        void topological_sort_sinks_queue(const G& g) {
            std::cout << "topological sort (sinks queue)" << std::endl;
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
            for (size_t index = 0; !queue.empty(); ++index) {
                auto v = queue.pop_front();
                ordered[index] = *v;
                for (auto w = v->cbegin(); w != v->cend(); ++w)
                    if (--in[*w] == 0)
                        queue.push_back(&*w);
            }
            std::cout << ordered << std::endl;
        }

    template<typename G, typename V = typename G::vertex_type>
        Array<size_t> strong_components_kosaraju(const G& g) {
            using namespace Dfs;
            struct Foo {
                const G& g_;
                Array<size_t> ids_;
                size_t group_id_;
                Foo(const G& g) 
                    :g_(g), ids_(g.vertices_count()), group_id_(-1) 
                { 
                    ids_.fill(-1);
                }
                void search() {
                    auto t_order = topological_sort_relabel(g_);
                    for (auto v = g_.crbegin(); v != g_.crend(); ++v) {
                        auto& t = g_.vertex_at(t_order[*v]);
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

            Foo f(g);
            f.search();
            return std::move(f.ids_);
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
}

