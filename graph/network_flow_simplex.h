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
                    inline T current_version() const { return current_version_; }

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
                    Array<typename vertex_type::edge_value_type> potentials_; // TODO move potentials here
                public:
                    Parent_link_array_tree(size_t size) :links_(size, nullptr), potentials_(size) {}
                    L*& operator[](size_t index) { return links_[index]; }
                    vertex_type* get_parent(vertex_type* v) { return &links_[*v]->other(*v); }
                    template<typename VA>
                    auto get_vertex_potential(vertex_type& v, VA& versions) {
                        auto link = links_[v];
                        if (!link)
                            return 0; // root potential (doesn't have a parent)
                        if (versions.is_up_to_date(v))
                            return potentials_[v];
                        potentials_[v] = get_vertex_potential(link->other(v), versions) - cost_r_to(*link, v);
                        versions.set_up_to_date(v);
                        return potentials_[v];
                    }
                    template<typename VA>
                        vertex_type* find_lca(vertex_type* v, vertex_type* w, VA& versions) {
                            ++versions;
                            versions.set_up_to_date(*v);
                            versions.set_up_to_date(*w);
                            auto step = [this, &versions](auto& v) {
                                auto link = links_[*v];
                                if (!link)
                                    return false; // tree root reached
                                v = &link->other(*v);;
                                if (links_[*v] && versions.is_up_to_date(*v))
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
                            std::cout << "lca: " << *v << std::endl;
                            return v;
                        }
                    bool is_between(vertex_type* descendant, vertex_type* const ancestor, vertex_type* const v) {
                        for (; descendant != ancestor; descendant = get_parent(descendant))
                            if (descendant == v)
                                return true;
                        return false;
                    }

                    template<typename F>
                    void calculate_augmentation_cap(vertex_type* v, vertex_type* lca,
                                                    typename vertex_type::edge_value_type& min_cap, F f) {
                        for (; v != lca; v = get_parent(v)) {
                            auto l = links_[*v];
                            auto w = f(l, v);
                            auto cap = l->cap_r_to(*w);
                            if (min_cap > cap)
                                min_cap = cap;
                        }
                    }

                    template<typename VA>
                    L* augment(L* link, VA& versions_) {
                        auto v = &link->source();
                        auto w = &link->target(); // todo add link method returning vertex pointer
                        auto lca = find_lca(v, w, versions_);

                        auto min_cap = link->cap_r_to(*w);
                        for (auto vv = w; vv != lca; ) {
                            auto l = links_[*vv];
                            auto p = &l->other(*vv);
                            auto cap = l->cap_r_to(*p);
                            if (min_cap > cap) min_cap = cap;
                            vv = p;
                        }
                        for (auto vv = v; vv != lca; vv = get_parent(vv)) {
                            auto cap = links_[*vv]->cap_r_to(*vv);
                            if (min_cap > cap) min_cap = cap;
                        }

                        link->add_flow_r_to(*w, min_cap);
                        auto old_link = link;
                        for (auto vv = w; vv != lca; ) {
                            auto l = links_[*vv];
                            auto p = &l->other(*vv);
                            l->add_flow_r_to(*p, min_cap);
                            if (l->cap_r_to(*p) == 0)
                                old_link = l;
                            vv = p;
                        }
                        for (auto vv = v; vv != lca; ) {
                            auto l = links_[*vv];
                            l->add_flow_r_to(*vv, min_cap);
                            if (l->cap_r_to(*vv) == 0)
                                old_link = l;
                            vv = &l->other(*vv);
                        }
                        // if (min_cap != 0) {
                            // std::cout << *old_link << " " << *link;
                            std::cout << "old link: " << *old_link;
                            std::cout << ", flow added: " << min_cap << std::endl;
                            // print_representation(g_, std::cout);
                        // }
                        return old_link;
                    }
                    template<typename VA>
                        void replace(L* old_link, L* new_link, VA& versions_array) {
                            std::cout << *this << std::endl;
                            std::cout << *old_link << "(old), " << *new_link << std::endl;
                            auto lca = find_lca(&new_link->source(), &new_link->target(), versions_array);

                            auto old_target = &old_link->target();
                            {
                                auto target_link = links_[*old_target];
                                if (!target_link || &target_link->target() != old_target)
                                    old_target = &old_link->source();
                            }

                            // auto old_target = &old_link->target();
                            // {
                            //     auto target_link = links_[*old_target];
                            //     if (&l->target() != old_target)
                            //         old_target = &old_link->source();
                            // }

                            std::cout << "old target: " << *old_target << std::endl;

                            auto new_target = &new_link->source();
                            if (is_between(&new_link->target(), lca, old_target))
                                new_target = &new_link->target();

                            std::cout << "new target: " << *new_target << std::endl;

                            auto prev_link = links_[*new_target];
                            for (auto v = get_parent(new_target); ;) {
                                auto p = get_parent(v);
                                std::swap(links_[*v], prev_link);
                                if (v == old_target) break;
                                v = p;
                            }

                            // std::cout << *this << std::endl;

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

                    // todo deduct types?
                    Parent_link_array_tree<link_type> tree_;
                    Versions_array<unsigned int> versions_;

                    link_type* aux_link_;

                public:
                    Simplex(G& g, vertex_type& s, vertex_type& t, const w_t& sentinel)
                        :Simplex(g, s, t, sentinel, g.vertices_count()) {}
                private:
                    Simplex(G& g, vertex_type& s, vertex_type& t, const w_t& sentinel, size_t v_count)
                        :g_(g), s_(s), t_(t), sentinel_(sentinel), 
                        tree_(v_count),
                        // st_(v_count, nullptr),
                        versions_(v_count, false)
                    {
                        aux_link_ = g.add_edge(t, s, sentinel, 180, 40); // todo hardcoded
                        dfs_r(aux_link_);
                        std::cout << "initial tree" << std::endl;
                        std::cout << tree_ << std::endl;

                        print_representation(g, std::cout);

                        foo();

                        print_representation(g, std::cout);
                    }
                    void foo() {
                        unsigned int old_v = 0;
                        while (versions_.current_version() != old_v) {
                            old_v = versions_.current_version();
                            for (auto& v : g_) {
                                for (auto e = v.edges_begin(); e != v.edges_end(); ++e) {
                                    auto link = e->edge().link();
                                    if (link->cap_r_to(link->other(v)) > 0 && link->cap_r_to(v) == 0) {
                                        if (!is_in_sp(link)) {
                                            std::cout << std::endl << tree_ << std::endl;
                                            std::cout << "adding: " << *link << std::endl;
                                            auto old_link = tree_.augment(link, versions_);
                                            tree_.replace(old_link, link, versions_);
                                            ++versions_;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    void dfs_r(link_type* link) {
                        auto& t = link->target();
                        tree_[t] = link;
                        for (auto e = t.edges_begin(); e != t.edges_end(); ++e) {
                            auto child_link = e->edge().link();
                            if (child_link->is_from(t) && !tree_[child_link->target()] && child_link->target() != t_)
                                dfs_r(child_link);
                        }
                    }
                    // void calculate_potentials() {
                    //     phi_[t_] = cost_r_to(*aux_link_, s_);
                    //     versions_.set_up_to_date(t_);
                    //     for (auto& v : g_)
                    //         if (v != t_)
                    //             phi_[v] = phi_r(v);
                    // }
                    // int phi_r(vertex_type& v) {
                    //     if (versions_.is_up_to_date(v))
                    //         return phi_[v];
                    //     auto link = tree_[v];
                    //     if (link)
                    //         phi_[v] = phi_r(*tree_.get_parent(&v)) - cost_r_to(*link, v);
                    //     versions_.set_up_to_date(v);
                    //     return phi_[v];
                    // }
                    auto cost_r(link_type* link, vertex_type& v) {
                        auto cost = link->cost() + tree_.get_vertex_potential(link->target()) 
                            - tree_.get_vertex_potential(link->source());
                        if (link->is_to(v)) cost *= -1;
                        return cost;
                    }
                    bool is_in_sp(link_type* l) { // todo is this check required?
                        return l == tree_[l->source()] || l == tree_[l->target()];
                    }
                    link_type* find_best_eligible() {
                        int min_cost = 40; // hardcoded
                        bool all_zeros = true;
                        link_type* link = nullptr;
                        for (auto& v : g_) {
                            for (auto e = v.edges_begin(); e != v.edges_end(); ++e) {
                                auto l = e->edge().link();
                                // if (l->cap_r_to(l->other(v)) > 0 && l->cap_r_to(v) == 0 && !is_in_sp(l)) {
                                if (l->cap_r_to(l->other(v)) > 0 && l->cap_r_to(v) == 0) {
                                    auto cost = cost_r(l, v); // todo how cost can be negative here?
                                    // std::cout << *l << ", cost: " << cost << std::endl;
                                    if (cost != 0)
                                        all_zeros = false;
                                    if (cost != 0 && min_cost > cost) {
                                        min_cost = cost;
                                        link = l;
                                    }
                                }
                            }
                        }
                            if (all_zeros)
                                min_cost = 0;
                            if (link)
                                std::cout << *link << ", r cost: " << min_cost << std::endl;
                        return link;
                    }
            };

    }
}

