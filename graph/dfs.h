#pragma once

#include "array.h"

namespace Graph {

    template<typename T>
        class Counters : public Array<T> {
            private:
                T current_max_;
            public:
                using value_type = T;
                static constexpr T default_value() { return static_cast<T>(-1); }
                Counters(size_t size) :Array<T>(size), current_max_(default_value()) { 
                    Array<T>::fill(default_value());
                }
                bool is_unset(size_t index) {
                    return Array<T>::operator[](index) == default_value();
                }
                void set_next(size_t index) {
                    Array<T>::operator[](index) = ++current_max_;
                }
        };

    template<>
        class Counters<bool> : public Array<bool> {
            public:
                using value_type = bool;
                static constexpr bool default_value() { return false; }
                Counters(size_t size) :Array<bool>(size) { Array<bool>::fill(default_value()); }
                bool is_unset(size_t index) {
                    return Array<bool>::operator[](index) == default_value();
                }
                void set_next(size_t index) {
                    Array<bool>::operator[](index) = true;
                }
        };

    namespace Dfs {

        template<typename G, typename T_pre = size_t>
            struct Base_dfs {
                using V = typename G::Vertex;
                using pre_counters_type = Counters<T_pre>;
                pre_counters_type pre_;
                Base_dfs(const G& g) :pre_(g.vertices_count()) {}
                void visit_vertex(const V&) {}
                void visit_edge(const V&, const V&) {}
                void search_post_process(const V&) {}
            };

        template<typename G, typename T_pre = size_t, typename T_post = size_t>
            struct Post_dfs : public Base_dfs<G, T_pre> {
                using V = typename G::Vertex;
                using post_counters_type = Counters<T_post>;
                post_counters_type post_;
                Post_dfs(const G& g) : Base_dfs<G, T_pre>(g), post_(g.vertices_count()) {}
                void search_post_process(const V& v) {
                    post_.set_next(v);
                }
            };

        template<typename G, typename DFS = Base_dfs<G>>
            struct Dfs_searcher : public DFS {
                using V = typename G::Vertex;
                const G& g_;
                template<typename... Args>
                    Dfs_searcher(const G& g, Args&&... args) 
                    :DFS(g, std::forward<Args>(args)...), g_(g) 
                    {}
                void search_pairs() {
                    for (auto v = g_.cbegin(); v != g_.cend(); ++v)
                        if (DFS::pre_.is_unset(*v))
                            search_pairs(*v, *v);
                }
                void search_pairs(const V& v, const V& w) {
                    DFS::visit_vertex(w);
                    DFS::pre_.set_next(w);
                    for (auto t = w.cbegin(); t != w.cend(); ++t) {
                        if (DFS::pre_.is_unset(*t))
                            search_pairs(w, *t);
                        DFS::visit_edge(w, *t);
                    }
                    DFS::search_post_process(w);
                }
                void search() {
                    for (auto v = g_.cbegin(); v != g_.cend(); ++v)
                        if (DFS::pre_.is_unset(*v))
                            search(*v);
                }
                void search(const V& v) {
                    DFS::pre_.set_next(v);
                    for (auto t = v.cbegin(); t != v.cend(); ++t)
                        if (DFS::pre_.is_unset(*t))
                            search(*t);
                    DFS::search_post_process(v);
                }
            };

    }

    template<typename G, typename V = typename G::Vertex, typename T_v_visitor, typename T_e_visitor>
        void dfs(const G& g, T_v_visitor v_visitor, T_e_visitor e_visitor) {
            using namespace Dfs;
            struct Searcher : public Base_dfs<G, bool> {
                T_v_visitor v_visitor_;
                T_e_visitor e_visitor_;
                Searcher(const G& g, T_v_visitor v_visitor, T_e_visitor e_visitor) 
                    :Base_dfs<G>(g), v_visitor_(v_visitor), e_visitor_(e_visitor)
                {}
                void visit_vertex(const V& v) {
                    v_visitor_(v);
                }
                void visit_edge(const V& v, const V& w) {
                    e_visitor_(v, w);
                }
            };
            Dfs_searcher<G, Searcher>(g, v_visitor, e_visitor).search_pairs();
        }

}
