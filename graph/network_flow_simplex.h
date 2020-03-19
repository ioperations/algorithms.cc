#pragma once

namespace Graph {
    namespace Network_flow_ns {
        template<typename G>
            class Simplex {
                private:
                    using vertex_type = typename G::vertex_type;
                    using w_t = typename G::edge_type::value_type;
                    using link_type = typename G::link_type;

                    G& g_;
                    vertex_type& s_;
                    vertex_type& t_;
                    const w_t sentinel_;

                    Array<link_type*> st_;
                    // todo deduct types?
                    int valid_;
                    Array<int> mark_, phi_; // todo rename to potentials?

                    link_type* aux_link_;

                public:
                    Simplex(G& g, vertex_type& s, vertex_type& t, const w_t& sentinel)
                        :Simplex(g, s, t, sentinel, g.vertices_count()) {}
                private:
                    Simplex(G& g, vertex_type& s, vertex_type& t, const w_t& sentinel, size_t v_count)
                        :g_(g), s_(s), t_(t), sentinel_(sentinel), st_(v_count, nullptr), mark_(v_count, -1), phi_(v_count)
                    {
                        aux_link_ = g.add_edge(t, s, sentinel, sentinel, sentinel);
                        print_representation(g, std::cout);

                        dfs_r(aux_link_);
                        print_st();

                        for (valid_ = 1; ; ++valid_) { // todo mark_ unsigned? rename?
                            calculate_potentials();
                            std::cout << phi_ << std::endl;

                            auto link = find_best_eligible();
                            auto link_cost = cost_r(link, link->source());
                            std::cout << "best eligible link: " << link->source() << " - " << link->target() <<
                                ", cost: " << link_cost << std::endl;

                            if (link_cost == 0)
                                break;

                            auto old_link = augment(link);
                            update(old_link, link);

                            print_st();
                        }

                        // std::cout << phi_ << std::endl;

                        print_representation(g, std::cout);

                    }
                    void print_st() {
                        for (auto l : st_)
                            if (l)
                                std::cout << l->source() << " - " << l->target() << ", ";
                            else
                                std::cout << "none, ";
                        std::cout << std::endl;
                    }
                    void dfs_r(link_type* link) {
                        auto& t = link->target();
                        st_[t] = link;
                        for (auto e = t.edges_begin(); e != t.edges_end(); ++e) {
                            auto child_link = e->edge().link();
                            if (child_link->is_from(t) && !st_[child_link->target()] && child_link->target() != t_)
                                dfs_r(child_link);
                        }
                    }
                    vertex_type& st(vertex_type& v) { return st_[v]->other(v); } // todo rename
                    void calculate_potentials() {
                        phi_[t_] = cost_r_to(*aux_link_, s_);
                        mark_[t_] = valid_;
                        for (auto& v : g_)
                            if (v != t_)
                                phi_[v] = phi_r(v);
                    }
                    int phi_r(vertex_type& v) {
                        if (mark_[v] == valid_)
                            return phi_[v];
                        phi_[v] = phi_r(st(v)) - cost_r_to(*st_[v], v);
                        mark_[v] = valid_;
                        return phi_[v];
                    }
                    auto cost_r(link_type* link, vertex_type& v) {
                        auto cost = link->cost() + phi_[link->target()] - phi_[link->source()];
                        if (link->is_to(v)) cost *= -1;
                        return cost;
                    }
                    link_type* find_best_eligible() {
                        int min_cost = sentinel_;
                        link_type* link = nullptr;
                        for (auto& v : g_)
                            for (auto e = v.edges_begin(); e != v.edges_end(); ++e) {
                                auto l = e->edge().link();
                                if (l->cap_r_to(l->other(v)) > 0 && l->cap_r_to(v) == 0) {
                                    auto cost = cost_r(l, v);
                                    if (cost < min_cost) {
                                        link = l;
                                        min_cost = cost;

                                    }
                                }
                            }
                        return link;
                    }
                    link_type* augment(link_type* link) {
                        auto v = &link->source();
                        auto w = &link->target(); // todo add link method returning vertex pointer
                        auto lca = find_lca(v, w);
                        auto flow = link->cap_r_to(*w);
                        auto calculate_cap = [this, &flow, lca](auto v) {
                            while (v != lca) {
                                auto l = st_[*v];
                                auto& w = l->other(*v);
                                auto cap = l->cap_r_to(*v);
                                if (flow > cap)
                                    flow = cap;
                                v = &w;
                            }
                        };
                        calculate_cap(w);
                        calculate_cap(v);

                        link->add_flow_r_to(*w, flow);
                        auto old_link = link;
                        auto add_flow = [this, &flow, lca, &old_link](auto v) {
                            while (v != lca) {
                                auto l = st_[*v];
                                auto& w = l->other(*v);
                                l->add_flow_r_to(*v, flow);
                                if (l->cap_r_to(*v) == 0)
                                    old_link = st_[*v];
                                v = &w;
                            }
                        };
                        add_flow(w);
                        add_flow(v);
                        std::cout << "find_lca: " << *lca << ", min flow: " << flow << ", old link: " <<
                            old_link->source() << " - " << old_link->target() << std::endl;
                        return old_link; // todo rename?
                    }
                    vertex_type* find_lca(vertex_type* v, vertex_type* w) {
                        mark_[*v] = ++valid_;
                        mark_[*w] = valid_;
                        auto try_next = [this](auto& v) -> bool {
                            if (*v != t_) v = &st(*v);
                            if (*v != t_ && mark_[*v] == valid_)
                                return true;
                            else {
                                mark_[*v] = valid_;
                                return false;
                            }
                        };
                        while (v != w) {
                            if (try_next(v)) return v;
                            if (try_next(w)) return w;
                        }
                        return v;
                    }
                    void update(link_type* old_link, link_type* link) {
                        auto s = &link->source();
                        auto t = &link->target();
                        auto old_t = &old_link->target();
                        auto lca = find_lca(t, s); // todo s, t?

                        auto do_update = [this, lca, old_t, link](auto v) {
                            if (on_path(v, lca, old_t)) {
                                reverse(v, old_t);
                                st_[*v] = link;
                                return true;
                            }
                            return false;
                        };

                        if (!do_update(t))
                            do_update(s);

                        // if (on_path(t, lca, old_t)) {
                        //     reverse(t, old_t);
                        //     st_[*t] = link;
                        //     return;
                        // }
                        // if (on_path(s, lca, old_t)) {
                        //     reverse(s, old_t);
                        //     st_[*s] = link;
                        //     return;
                        // }
                    }
                    bool on_path(vertex_type* s, vertex_type* t, vertex_type* v) {
                        for (; s != t; s = &st(*s))
                            if (s == v)
                                return true;
                        return false;
                    }
                    void reverse(vertex_type* s, vertex_type* t) {
                        auto link = st_[*s];
                        for (auto i = &st(*s); i != t; i = &st(*i)) {
                            auto old_link = st_[*i];
                            st_[*i] = link;
                            link = old_link;
                        }
                    }
            };

    }
}

