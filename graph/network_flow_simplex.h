#pragma once

#include "array.h"

namespace Graph {

namespace Network_flow_ns {

template <typename T>
class Versions_array {
   private:
    T current_version_;
    Array<T> versions_;

   public:
    Versions_array(size_t size, bool up_to_date = true)
        : current_version_(0), versions_(size, 0) {
        if (!up_to_date) this->operator++();
    }
    Versions_array& operator++() {
        ++current_version_;
        return *this;
    }
    inline void set_up_to_date(size_t index) {
        versions_[index] = current_version_;
    }
    inline bool is_up_to_date(size_t index) {
        return versions_[index] == current_version_;
    }
    inline T current_version() const { return current_version_; }

    template <typename TT>
    friend std::ostream& operator<<(std::ostream& stream,
                                    const Versions_array<TT>& a) {
        return stream << a.versions_ << " (" << a.current_version_ << ")";
    }
};

template <typename L>
class Parent_link_array_tree {
   private:
    using vertex_type = typename L::vertex_type;
    using potential_type = typename vertex_type::edge_value_type;

    Array<L*> links_;
    Versions_array<unsigned int> versions_;
    Array<potential_type> potentials_;

   public:
    Parent_link_array_tree(size_t size)
        : links_(size, nullptr), versions_(size, false), potentials_(size) {}
    L*& operator[](size_t index) { return links_[index]; }
    vertex_type* get_parent(vertex_type* v) { return &links_[*v]->other(*v); }
    auto current_version() { return versions_.current_version(); }
    const Array<potential_type>& potentials() { return potentials_; }
    auto get_vertex_potential(vertex_type& v) {
        auto link = links_[v];
        if (!link) return 0;  // root potential (doesn't have a parent)
        if (versions_.is_up_to_date(v)) return potentials_[v];
        potentials_[v] =
            get_vertex_potential(link->other(v)) - cost_r_to(*link, v);
        versions_.set_up_to_date(v);
        return potentials_[v];
    }
    vertex_type* find_lca(L* link) {
        auto v = &link->source();
        auto w = &link->target();
        ++versions_;
        versions_.set_up_to_date(*v);
        versions_.set_up_to_date(*w);
        auto step = [this](auto& v) {
            auto link = links_[*v];
            if (!link) return false;  // tree root reached
            v = &link->other(*v);
            ;
            if (links_[*v] && versions_.is_up_to_date(*v))
                return true;
            else {
                versions_.set_up_to_date(*v);
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

    bool is_between(vertex_type* descendant, vertex_type* const ancestor,
                    vertex_type* const v) {
        for (; descendant != ancestor; descendant = get_parent(descendant))
            if (descendant == v) return true;
        return false;
    }

    L* augment(L* link, vertex_type* lca) {
        auto v = &link->source();
        auto w =
            &link->target();  // todo add link method returning vertex pointer

        auto min_cap = link->cap_r_to(*w);
        for (auto vv = w; vv != lca;) {
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
        for (auto vv = w; vv != lca;) {
            auto l = links_[*vv];
            auto p = &l->other(*vv);
            l->add_flow_r_to(*p, min_cap);
            if (l->cap_r_to(*p) == 0) old_link = l;
            vv = p;
        }
        for (auto vv = v; vv != lca;) {
            auto l = links_[*vv];
            l->add_flow_r_to(*vv, min_cap);
            if (l->cap_r_to(*vv) == 0) old_link = l;
            vv = &l->other(*vv);
        }
        return old_link;
    }
    void replace(L* old_link, L* new_link, vertex_type* lca) {
        auto old_target = &old_link->target();
        if (old_link != links_[*old_target]) old_target = &old_link->source();

        auto new_target = &new_link->source();
        if (is_between(&new_link->target(), lca, old_target))
            new_target = &new_link->target();

        if (new_target != old_target) {
            auto prev_link = links_[*new_target];
            for (auto v = get_parent(new_target);;) {
                auto p = get_parent(v);
                std::swap(links_[*v], prev_link);
                if (v == old_target) break;
                v = p;
            }
        }
        links_[*new_target] = new_link;
        ++versions_;
    }
    template <typename LL>
    friend std::ostream& operator<<(std::ostream& stream,
                                    const Parent_link_array_tree<LL>& tree) {
        if (tree.links_.size() != 0) {
            auto print = [&stream, &tree](size_t i) {
                auto link = tree.links_[i];
                if (link) {
                    auto v = &link->source();
                    auto w = &link->target();
                    if (w->index() != i) std::swap(v, w);
                    stream << *v << "-" << *w;
                } else
                    stream << "--" << i;
            };
            print(0);
            for (size_t i = 1; i < tree.links_.size(); ++i) {
                stream << ", ";
                print(i);
            }
        }
        return stream;
    }
};

template <typename G>
class Simplex {
   private:
    using vertex_type = typename G::vertex_type;
    using w_t = typename G::edge_type::value_type;
    using link_type = typename G::link_type;

    Parent_link_array_tree<link_type> tree_;

   public:
    Simplex(G& g, vertex_type& s, vertex_type& t, const w_t& sentinel)
        : Simplex(g, s, t, sentinel, g.vertices_count()) {}

   private:
    Simplex(G& g, vertex_type& s, vertex_type& t, const w_t& sentinel,
            size_t v_count)
        : tree_(v_count) {
        tree_[t] = g.add_edge(s, t, sentinel, sentinel, sentinel);
        add_to_tree(s);

        // std::cout << std::endl << std::endl;
        // print_representation(g, std::cout);
        // std::cout << tree_ << std::endl;

        for (decltype(tree_.current_version()) old_version = 0;
             old_version != tree_.current_version();) {
            old_version = tree_.current_version();
            for (auto& v : g) {
                for (auto e = v.edges_begin(); e != v.edges_end(); ++e) {
                    auto link = e->edge().link();
                    if (link->cap_r_to(link->other(v)) > 0 &&
                        link->cap_r_to(v) == 0) {
                        auto cost = residual_cost(link, v);
                        if (cost < 0) {
                            auto lca = tree_.find_lca(link);
                            auto old_link = tree_.augment(link, lca);
                            // std::cout << std::endl
                            //     << tree_ << std::endl
                            //     << tree_.potentials() << std::endl
                            //     << "eligible: " << *link << ", r. cost: " <<
                            //     cost
                            //     << ", lca: " << *lca << ", old edge: " <<
                            //     *old_link << std::endl;
                            tree_.replace(old_link, link, lca);
                            // print_representation(g, std::cout);
                        }
                    }
                }
            }
        }

        g.remove_edge(s, t);
    }
    void add_to_tree(vertex_type& v) {
        for (auto e = v.edges_begin(); e != v.edges_end(); ++e) {
            auto l = e->edge().link();
            if (l->is_from(v)) {
                auto& w = l->other(v);
                auto& existing_l = tree_[w];
                if (!existing_l) {
                    existing_l = l;
                    add_to_tree(w);
                }
            }
        }
    }
    auto residual_cost(link_type* link, vertex_type& v) {
        auto cost = link->cost() + tree_.get_vertex_potential(link->target()) -
                    tree_.get_vertex_potential(link->source());
        if (link->is_to(v)) cost *= -1;
        return cost;
    }
};
}  // namespace Network_flow_ns
}  // namespace Graph
