#pragma once

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
                using Vertex = typename G::Vertex;
                G& graph_;
                std::map<T, typename G::Vertex&> vertices_;
            public:
                Constructor(G& graph) :graph_(graph) {}
                Constructor& add_edge(const T& l1, const T& l2) {
                    auto& v1 = get_or_create_vertex(l1);
                    auto& v2 = get_or_create_vertex(l2);
                    graph_.add_edge(v1, v2);
                    return *this;
                }
                Vertex& get_or_create_vertex(const T& l) {
                    auto it = vertices_.find(l);
                    if (it == vertices_.end()) {
                        auto& v = graph_.create_vertex(l);
                        vertices_.insert({l, v});
                        return v;
                    } else
                        return it->second;
                }
                Vertex& get_vertex(const T& label) {
                    auto it = vertices_.find(label);
                    if (it == vertices_.end())
                        throw std::runtime_error("vertex "_str + label + " not found");
                    return it->second;
                };
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
                size_t index_;

                Vertex_base(const T& value, size_t index) 
                    :value_(value), index_(index) 
                {}
                Vertex_base() = default;
            public:
                const T& value() const {
                    return value_;
                }
                size_t index() const {
                    return index_;
                }
                operator size_t() const {
                    return index_;
                }
                bool operator==(const Vertex_base& o) const {
                    return index_ == o.index_;
                }
                bool operator!=(const Vertex_base& o) const {
                    return !operator==(o);
                }
                friend std::ostream& operator<<(std::ostream& stream, const Vertex_base& v) {
                    return stream << v.value_;
                }
        };

    template<typename V>
        auto count_vertex_edges(const V& v) {
            size_t count = 0;
            for (auto e = v.cbegin(); e != v.cend(); ++e)
                ++count;
            return count;
        }

    template<typename G, typename V = typename G::Vertex>
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

    template<typename G, typename V = typename G::Vertex>
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

    template<typename G, typename V = typename G::Vertex>
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

    template<typename G, typename V = typename G::Vertex, typename T_v_visitor, typename T_e_visitor>
        void dfs(const G& g, T_v_visitor v_visitor, T_e_visitor e_visitor) {
            if (g.vertices_count() == 0)
                return;
            struct Helper {
                Array<bool> visited_;
                Helper(size_t size) :visited_(size) {
                    for (auto& v : visited_)
                        v = false;
                }
                void search(const V& v, T_v_visitor v_visitor, T_e_visitor e_visitor) {
                    auto& visited = visited_[v];
                    if (visited)
                        return;
                    v_visitor(v);
                    visited = true;
                    for (auto w = v.cbegin(); w != v.cend(); ++w) {
                        search(*w, v_visitor, e_visitor);
                        e_visitor(v, *w);
                    }
                }
                void search(const G& g, T_v_visitor v_visitor, T_e_visitor e_visitor) {
                    for (auto v = g.cbegin(); v != g.cend(); ++v)
                        search(*v, v_visitor, e_visitor);
                }
            };
            Helper(g.vertices_count()).search(g, v_visitor, e_visitor);
        }

    template<typename G, typename V = typename G::Vertex>
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

    template<typename G, typename V = typename G::Vertex>
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

    template<typename G, typename V = typename G::Vertex>
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

    template<typename T, bool T_inverted = false>
        class Counters : public Array<T> {
            private:
                T current_max_;
            public:
                Counters(size_t size) :Array<T>(size), current_max_(-1) { Array<T>::fill(-1); }
                bool is_unset(size_t index) {
                    return Array<int>::operator[](index) == -1;
                }
                void set_next(size_t index) {
                    Array<int>::operator[](index) = ++current_max_;
                }
        };

    template<typename G, typename V = typename G::Vertex>
        void trace_dfs(const G& g) {
            struct S {
                const G& g_;
                Counters<int> pre_;
                Counters<int> post_;
                int depth_;
                S(const G& g, size_t size) :g_(g), pre_(size), post_(size), depth_(-1) {}
                void search() {
                    for (auto v = g_.cbegin(); v != g_.cend(); ++v)
                        if (pre_.is_unset(*v)) {
                            std::cout << *v << std::endl;
                            search(*v, *v);
                        }
                }
                void search(const V& v, const V& w) {
                    pre_.set_next(w);
                    ++depth_;
                    for (auto it = w.cbegin(); it != w.cend(); ++it) {
                        auto& t = *it;
                        if (pre_.is_unset(t))
                            search(w, t);
                        if (pre_[t] > pre_[w])
                            print("down", w, t);
                        else if (post_[t] == -1)
                            print("back", w, t);
                        else
                            print("cross", w, t);
                    }
                    post_.set_next(w);
                    --depth_;
                }
                void print(const char* label, const V& v, const V& w) {
                    for (int i = 0; i < depth_; ++i) std::cout << " ";
                    std::cout << label << " " << v << " " << w << std::endl;
                }
            };
            S s(g, g.vertices_count());
            s.search();
        }

    template<typename G, typename V = typename G::Vertex>
        bool is_dag(const G& g) {
            struct Searcher {
                const G& g_;
                Graph::Counters<int> pre_;
                Graph::Counters<int> post_;
                bool dag_is_valid_;
                Searcher(const G& g, size_t size) 
                    :g_(g), pre_(size), post_(size), dag_is_valid_(true) 
                {}
                void search() {
                    for (auto v = g_.cbegin(); v != g_.cend(); ++v)
                        if (dag_is_valid_ && pre_.is_unset(*v))
                            search(*v, *v);
                }
                void search(const V& v, const V& w) {
                    pre_.set_next(w);
                    for (auto it = w.cbegin(); dag_is_valid_ && it != w.cend(); ++it) {
                        auto& t = *it;
                        if (pre_.is_unset(t))
                            search(w, t);
                        if (pre_[t] < pre_[w] && post_.is_unset(t))
                            // back link found
                            dag_is_valid_ = false;
                    }
                    post_.set_next(w);
                }
            };
            Searcher s(g, g.vertices_count());
            s.search();
            return s.dag_is_valid_;
        }

}

