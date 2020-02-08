#pragma once

#include "array.h"

namespace Graph {

    template<typename T>
        class Counters : public Array<T> {
            private:
                using Base = Array<T>;
                T current_max_;
            public:
                using value_type = T;
                static constexpr T default_value() { return static_cast<T>(-1); }
                Counters(size_t size) :Base(size), current_max_(default_value()) { 
                    Base::fill(default_value());
                }
                bool is_unset(size_t index) {
                    return Base::operator[](index) == default_value();
                }
                void set_next(size_t index) {
                    Base::operator[](index) = ++current_max_;
                }
                Array<T> to_array() {
                    Array<T> array(std::move(*this));
                    return array;
                }
        };

    template<>
        class Counters<bool> : public Array<bool> {
            private:
                using Base = Array<bool>;
            public:
                using value_type = bool;
                static constexpr bool default_value() { return false; }
                Counters(size_t size) :Base(size) { Base::fill(default_value()); }
                bool is_unset(size_t index) {
                    return Base::operator[](index) == default_value();
                }
                void set_next(size_t index) {
                    Base::operator[](index) = true;
                }
        };

    namespace Dfs {

        template<typename G, typename T_pre = size_t>
            struct Base_dfs {
                using V = typename G::vertex_type;
                using pre_counters_type = Counters<T_pre>;
                const G& g_;
                pre_counters_type pre_;
                Base_dfs(const G& g) :g_(g), pre_(g.vertices_count()) {}
                void visit_vertex(const V&) {}
                void visit_edge(const V&, const V&) {}
                void search_post_process(const V&) {}
            };

        template<typename G, typename T_pre = size_t, typename T_post = size_t>
            struct Post_dfs : public Base_dfs<G, T_pre> {
                using V = typename G::vertex_type;
                using post_counters_type = Counters<T_post>;
                post_counters_type post_;
                Post_dfs(const G& g) : Base_dfs<G, T_pre>(g), post_(g.vertices_count()) {}
                void search_post_process(const V& v) {
                    post_.set_next(v);
                }
            };

        template<typename G, typename DFS = Base_dfs<G>>
            struct Dfs_searcher : public DFS {
                using V = typename G::vertex_type;
                template<typename... Args>
                    Dfs_searcher(const G& g, Args&&... args) 
                    :DFS(g, std::forward<Args>(args)...) 
                    {}
                void search_pairs() {
                    for (auto v = DFS::g_.cbegin(); v != DFS::g_.cend(); ++v)
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
                    for (auto v = DFS::g_.cbegin(); v != DFS::g_.cend(); ++v)
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

    template<typename G, typename V = typename G::vertex_type, typename T_v_visitor, typename T_e_visitor>
        void dfs(const G& g, T_v_visitor v_visitor, T_e_visitor e_visitor) {
            using namespace Dfs;
            struct Searcher : public Base_dfs<G, bool> {
                T_v_visitor v_visitor_;
                T_e_visitor e_visitor_;
                Searcher(const G& g, T_v_visitor v_visitor, T_e_visitor e_visitor) 
                    :Base_dfs<G, bool>(g), v_visitor_(v_visitor), e_visitor_(e_visitor)
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

    template<typename G, typename T_pre, typename D>
        class Weighted_dfs_base {
            private:
                const G& g_;
            protected:
                using vertex_type = typename G::vertex_type;
                using edge_type = typename G::vertex_type::const_edges_iterator::entry_type;
                Counters<T_pre> pre_;
            public:
                Weighted_dfs_base(const G& g) :g_(g), pre_(g.vertices_count()) {}
                void search() {
                    for (auto v = g_.cbegin(); v != g_.cend(); ++v)
                        if (pre_.is_unset(*v))
                            search(*v);
                }
                void search(const vertex_type& v) {
                    static_cast<D*>(this)->visit_vertex(v);
                    pre_.set_next(v);
                    for (auto e = v.cedges_begin(); e != v.cedges_end(); ++e) {
                        if (pre_.is_unset(e->target()))
                            search(e->target());
                        static_cast<D*>(this)->visit_edge(*e);
                    }
                    static_cast<D*>(this)->search_post_process(v);
                }
        };

    template<typename G, typename T_pre, typename D>
        class Weighted_dfs : public Weighted_dfs_base<G, T_pre, D> {
            private:
                using Base = Weighted_dfs_base<G, T_pre, D>;
            public:
                Weighted_dfs(const G& g) :Base(g) {}
                void search_post_process(const typename Base::vertex_type& v) {}
        };

    template<typename G, typename T_pre, typename T_post, typename D>
        class Weighted_post_dfs : public Weighted_dfs_base<G, T_pre, D> {
            private:
                using Base = Weighted_dfs_base<G, T_pre, D>;
            protected:
                Counters<T_post> post_;
            public:
                Weighted_post_dfs(const G& g) :Base(g), post_(g.vertices_count()) {}
                void search_post_process(const typename Base::vertex_type& v) { post_.set_next(v); }
        };

    template<typename G, typename V = typename G::vertex_type, typename T_v_visitor, typename T_e_visitor>
        void dfs_2(const G& g, T_v_visitor v_visitor, T_e_visitor e_visitor) {
            class Searcher : public Weighted_dfs<G, bool, Searcher> {
                private:
                    using Base = Weighted_dfs<G, bool, Searcher>;
                    using vertex_type = typename Base::vertex_type;
                    using edge_type = typename Base::edge_type;
                    T_v_visitor v_visitor_;
                    T_e_visitor e_visitor_;
                public:
                    Searcher(const G& g, T_v_visitor v_visitor, T_e_visitor e_visitor) 
                        :Base(g), v_visitor_(v_visitor), e_visitor_(e_visitor)
                    {}
                    void visit_vertex(const vertex_type& v) { v_visitor_(v); }
                    void visit_edge(const edge_type& e) { e_visitor_(e); }
            };
            Searcher(g, v_visitor, e_visitor).search();
        }

}
