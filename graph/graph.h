#pragma once

#include "array.h"
#include "stack.h"

#include <iostream>

namespace Graph {

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
                T value() const {
                    return value_;
                }
                size_t index() const {
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
                    visited[v1.index()] = true;
                    for (const auto v = v1.cbegin(); v != v1.cend(); ++v)
                        if (!visited[v->index()])
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
                        visited[v1.index()] = true;
                        for (auto v = v1.cbegin(); v != v1.cend() && !has; ++v)
                            has = !visited[v->index()] 
                                && has_hamilton_path(graph, *v, v2, visited, depth - 1, stack);
                        if (!has)
                            visited[v1.index()] = false;
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
        bool compose_euler_tour(const G& g, const V& v1, const V& v2) {
            Array<size_t> degrees(g.vertices_count());
            {
                auto e = degrees.begin();
                for (auto v = g.cbegin(); v != g.cend(); ++v, ++e)
                    *e = count_vertex_edges(*v);
            }
            auto degree = degrees[v1.index()] + degrees[v2.index()];
            if (degree % 2 != 0) {
                return false;
            }

            for (auto v = g.cbegin(); v != g.cend(); ++v)
                if (*v != v1 && *v != v2)
                    if (degrees[v->index()] % 2 != 0)
                        return false;

            using Stack = Stack<const V*>;
            struct Helper {
                G& g_;
                Stack stack_;
                Helper(G& g) :g_(g) {}
                const V* tour(const V* v) {
                    while (true) {
                        auto w = v->cbegin();
                        if (w == v->cend())
                            break;
                        stack_.push(v);
                        g_.remove_edge(*v, *w);
                        v = &*w;
                    }
                    return v;
                }
                void show(const V* v) {
                    Forward_list<const V*> path;
                    path.push_back(v);
                    while (tour(v) == v && !stack_.empty()) {
                        v = stack_.pop();
                        path.push_back(v);
                    }
                    for (auto p : path)
                        std::cout << *p << " - ";
                    std::cout << std::endl;
                }
            };

            auto gg = g;
            Helper(gg).show(&gg.vertex_at(0));

            return true;
        }

    template<typename It, typename F>
        void print_collection(const It& b, const It& e, const char* del, F f, std::ostream& stream) {
            if (b == e)
                return;
            auto item = b;
            stream << f(*item);
            for (++item; item != e; ++item)
                stream << del << *item;
        }

    template<typename It>
        void print_collection(const It& b, const It& e, const char* del, std::ostream& stream) {
            print_collection(b, e, del, [](auto& v) { return v; }, stream);
        }


}

