#pragma once

#include "array.h"

namespace Graph {

    namespace Network_flow_ns {

        template<typename T>
            class Versions_array {
                private:
                    T current_version_;
                    Array<T> versions_;
                public:
                    Versions_array(size_t size, bool up_to_date = true) :current_version_(0), versions_(size, 0) {
                        if (!up_to_date)
                            this->operator++();
                    }
                    Versions_array& operator++() {
                        ++current_version_;
                        return *this;
                    }
                    inline void set_up_to_date(size_t index) { versions_[index] = current_version_; }
                    inline bool is_up_to_date(size_t index) { return versions_[index] == current_version_; }

                    template<typename TT>
                        friend std::ostream& operator<<(std::ostream& stream, const Versions_array<TT>& a) {
                            return stream << a.versions_ << " (" << a.current_version_ << ")";
                        }
            };

        template<typename L>
            class Parent_link_array_tree {
                private:
                    using vertex_type = typename L::vertex_type;
                    Array<L*> links_;
                public:
                    Parent_link_array_tree(size_t size) :links_(size, nullptr) {}
                    L*& operator[](size_t index) { return links_[index]; }
                    vertex_type* get_parent(vertex_type* v) { return &links_[*v]->other(*v); }
                    template<typename VA>
                        vertex_type* find_lca(vertex_type* v, vertex_type* w, vertex_type* root, VA& versions) {
                            ++versions;
                            versions.set_up_to_date(*v);
                            versions.set_up_to_date(*w);
                            auto step = [this, root, &versions](auto& v) {
                                if (v != root)
                                    v = get_parent(v);
                                if (v != root && versions.is_up_to_date(*v))
                                    return true;
                                else {
                                    versions.set_up_to_date(*v);
                                    return false;
                                }
                            };
                            while (v != w) {
                                if (step(v))
                                    w = v;
                                else if (step(w))
                                    v = w;
                            }
                            return v;
                        }
                    bool is_between(vertex_type* descendant, vertex_type* const ancestor, vertex_type* const v) {
                        for (; descendant != ancestor; descendant = get_parent(descendant))
                            if (descendant == v)
                                return true;
                        return false;
                    }
                    template<typename VA>
                        void replace(L* old_link, L* new_link, vertex_type* const root, VA& versions_array) {
                            auto lca = find_lca(&new_link->source(), &new_link->target(), root, versions_array);
                            auto new_target = &new_link->source();
                            if (is_between(&new_link->target(), lca, &old_link->target()))
                                new_target = &new_link->target();

                            auto current = get_parent(new_target);
                            auto prev = new_target;
                            auto prev_link = links_[*new_target];
                            while (prev != &old_link->target()) {
                                auto parent = get_parent(current);
                                std::swap(links_[*current], prev_link);
                                prev = current;
                                current = parent;
                            }
                            links_[*new_target] = new_link;
                        }
                    template<typename LL>
                        friend std::ostream& operator<<(std::ostream& stream, const Parent_link_array_tree<LL>& tree) {
                            if (tree.links_.size() != 0) {
                                auto print = [&stream, &tree](size_t i) {
                                    auto link = tree.links_[i];
                                    if (link) {
                                        auto v = &link->source();
                                        auto w = &link->target();
                                        if (w->index() != i)
                                            std::swap(v, w);
                                        stream << *v << "-" << *w;
                                    } else
                                        stream << "--" << i;
                                };
                                print(0);
                                for (size_t i = 1; i < tree.links_.size(); ++ i) {
                                    stream << ", ";
                                    print(i);
                                }
                            }
                            return stream;
                        }
            };

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
                        aux_link_ = g.add_edge(t, s, sentinel, 180, 40); // todo hardcoded
                        // print_representation(g, std::cout);
                        dfs_r(aux_link_);
                        print_st();
                        std::cout << std::endl;

                        int counter = 0;
                        for (valid_ = 1; counter < 10; ++valid_, ++counter) { // todo mark_ unsigned? rename?
                            // print_st();

                            calculate_potentials();
                            // std::cout << phi_ << std::endl;

                            auto link = find_best_eligible();
                            auto link_cost = cost_r(link, link->source());
                            // std::cout << "best eligible link: " << link->source() << " - " << link->target() <<
                            //     ", cost: " << link_cost << std::endl;

                            if (link_cost == 0)
                                break;

                            auto old_link = augment(link);
                            update(old_link, link);

                            print_st();
                            print_representation(g, std::cout);
                            std::cout << std::endl;
                        }

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
                    bool is_in_sp(link_type* l) { // todo is this check required?
                        return l == st_[l->source()] || l == st_[l->target()];
                    }
                    link_type* find_best_eligible() {
                        int min_cost = 40; // hardcoded
                        link_type* link = nullptr;
                        for (auto& v : g_)
                            for (auto e = v.edges_begin(); e != v.edges_end(); ++e) {
                                auto l = e->edge().link();
                                if (l->cap_r_to(l->other(v)) > 0 && l->cap_r_to(v) == 0 && !is_in_sp(l)) {
                                    auto cost = cost_r(l, v); // todo how cost can be negative here?
                                    if (min_cost > cost) {
                                        min_cost = cost;
                                        link = l;
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

                        std::cout << link->source() << " - " << link->target() <<
                            " : " << old_link->source() << " - " << old_link->target() <<
                            ", flow added: " << flow << std::endl;

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

                        // todo why double check on_path?
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
                        // std::cout << "on path" << std::endl;
                        for (; s != t; s = &st(*s))
                            if (s == v)
                                return true;
                        return false;
                    }
                    void reverse(vertex_type* s, vertex_type* t) {
                        // std::cout << "reverse: " << *s << " - " << *t << std::endl;
                        // if (s != t) { // todo refactor
                        auto link = st_[*s];
                        for (auto i = &st(*s); i != t; i = &st(*i)) {
                            // std::cout << *i << std::endl;
                            auto old_link = st_[*i];
                            st_[*i] = link;
                            link = old_link;
                        }
                        // }
                    }
            };

    }
}

