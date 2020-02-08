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

    template<typename G, typename T_pre, typename D>
        class Weighted_dfs_base {
            protected:
                using vertex_type = typename G::vertex_type;
                using edge_type = typename G::vertex_type::const_edges_iterator::entry_type;
                const G& g_;
                Counters<T_pre> pre_;
            public:
                Weighted_dfs_base(const G& g) :g_(g), pre_(g.vertices_count()) {}
                void search() {
                    for (auto v = g_.cbegin(); v != g_.cend(); ++v)
                        if (pre_.is_unset(*v))
                            search_vertex(*v);
                }
                void search_vertex(const vertex_type& v) {
                    static_cast<D*>(this)->visit_vertex(v);
                    pre_.set_next(v);
                    for (auto e = v.cedges_begin(); e != v.cedges_end(); ++e) {
                        if (pre_.is_unset(e->target()))
                            search_vertex(e->target());
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
        class Weighted_post_dfs_base : public Weighted_dfs_base<G, T_pre, D> {
            private:
                using Base = Weighted_dfs_base<G, T_pre, D>;
            protected:
                using vertex_type = typename Base::vertex_type;
                using edge_type = typename Base::edge_type;
            public:
                Counters<T_post> post_;
                Weighted_post_dfs_base(const G& g) :Base(g), post_(g.vertices_count()) {}
                void visit_vertex(const vertex_type& v) {}
                void visit_edge(const edge_type& e) {}
                void search_post_process(const vertex_type& v) { post_.set_next(v); }
        };

    template<typename G, typename T_pre, typename T_post>
        class Weighted_post_dfs : public Weighted_post_dfs_base<G, T_pre, T_post, Weighted_post_dfs<G, T_pre, T_post>> {
            private:
                using Base = Weighted_post_dfs_base<G, T_pre, T_post, Weighted_post_dfs<G, T_pre, T_post>>;
            public:
                Weighted_post_dfs(const G& g) :Base(g) {}
        };

    template<typename G, typename V = typename G::vertex_type, typename T_v_visitor, typename T_e_visitor>
        void dfs(const G& g, T_v_visitor v_visitor, T_e_visitor e_visitor) {
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
