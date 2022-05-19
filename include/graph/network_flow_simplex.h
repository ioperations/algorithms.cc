#pragma once

#include "array.h"

namespace Graph {

namespace Network_flow_ns {

template <typename T>
class VersionsArray {
   private:
    T m_current_version;
    Array<T> m_versions;

   public:
    VersionsArray(size_t size, bool up_to_date = true)
        : m_current_version(0), m_versions(size, 0) {
        if (!up_to_date) this->operator++();
    }
    VersionsArray& operator++() {
        ++m_current_version;
        return *this;
    }
    inline void set_up_to_date(size_t index) {
        m_versions[index] = m_current_version;
    }
    inline bool is_up_to_date(size_t index) {
        return m_versions[index] == m_current_version;
    }
    inline T current_version() const { return m_current_version; }

    template <typename TT>
    friend std::ostream& operator<<(
        std::ostream& stream,
        const ::Graph::Network_flow_ns::VersionsArray<TT>& a) {
        return stream << a.m_versions << " (" << a.m_current_version << ")";
    }
};

template <typename L>
class ParentLinkArrayTree {
   private:
    using vertex_type = typename L::vertex_type;
    using potential_type = typename vertex_type::edge_value_type;

    Array<L*> m_links;
    VersionsArray<unsigned int> m_versions;
    Array<potential_type> m_potentials;

   public:
    ParentLinkArrayTree(size_t size)
        : m_links(size, nullptr), m_versions(size, false), m_potentials(size) {}
    L*& operator[](size_t index) { return m_links[index]; }
    vertex_type* get_parent(vertex_type* v) { return &m_links[*v]->other(*v); }
    auto current_version() { return m_versions.current_version(); }
    const Array<potential_type>& potentials() { return m_potentials; }
    auto get_vertex_potential(vertex_type& v) {
        auto link = m_links[v];
        if (!link) return 0;  // root potential (doesn't have a parent)
        if (m_versions.is_up_to_date(v)) return m_potentials[v];
        m_potentials[v] =
            get_vertex_potential(link->other(v)) - cost_r_to(*link, v);
        m_versions.set_up_to_date(v);
        return m_potentials[v];
    }
    vertex_type* find_lca(L* link) {
        auto v = &link->source();
        auto w = &link->target();
        ++m_versions;
        m_versions.set_up_to_date(*v);
        m_versions.set_up_to_date(*w);
        auto step = [&link, this](auto& v) {
            auto link = m_links[*v];
            if (!link) return false;  // tree root reached
            v = &link->other(*v);
            ;
            if (m_links[*v] && m_versions.is_up_to_date(*v)) return true;
            m_versions.set_up_to_date(*v);
            return false;
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
            auto l = m_links[*vv];
            auto p = &l->other(*vv);
            auto cap = l->cap_r_to(*p);
            if (min_cap > cap) min_cap = cap;
            vv = p;
        }
        for (auto vv = v; vv != lca; vv = get_parent(vv)) {
            auto cap = m_links[*vv]->cap_r_to(*vv);
            if (min_cap > cap) min_cap = cap;
        }

        link->add_flow_r_to(*w, min_cap);
        auto old_link = link;
        for (auto vv = w; vv != lca;) {
            auto l = m_links[*vv];
            auto p = &l->other(*vv);
            l->add_flow_r_to(*p, min_cap);
            if (l->cap_r_to(*p) == 0) old_link = l;
            vv = p;
        }
        for (auto vv = v; vv != lca;) {
            auto l = m_links[*vv];
            l->add_flow_r_to(*vv, min_cap);
            if (l->cap_r_to(*vv) == 0) old_link = l;
            vv = &l->other(*vv);
        }
        return old_link;
    }
    void replace(L* old_link, L* new_link, vertex_type* lca) {
        auto old_target = &old_link->target();
        if (old_link != m_links[*old_target]) old_target = &old_link->source();

        auto new_target = &new_link->source();
        if (is_between(&new_link->target(), lca, old_target))
            new_target = &new_link->target();

        if (new_target != old_target) {
            auto prev_link = m_links[*new_target];
            for (auto v = get_parent(new_target);;) {
                auto p = get_parent(v);
                std::swap(m_links[*v], prev_link);
                if (v == old_target) break;
                v = p;
            }
        }
        m_links[*new_target] = new_link;
        ++m_versions;
    }
    template <typename LL>
    friend std::ostream& operator<<(std::ostream& stream,
                                    const ParentLinkArrayTree<LL>& tree) {
        if (tree.m_links.size() != 0) {
            auto print = [&stream, &tree](size_t i) {
                auto link = tree.m_links[i];
                if (link) {
                    auto v = &link->source();
                    auto w = &link->target();
                    if (w->index() != i) std::swap(v, w);
                    stream << *v << "-" << *w;
                } else
                    stream << "--" << i;
            };
            print(0);
            for (size_t i = 1; i < tree.m_links.size(); ++i) {
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

    ParentLinkArrayTree<link_type> m_tree;

   public:
    Simplex(G& g, vertex_type& s, vertex_type& t, const w_t& sentinel)
        : Simplex(g, s, t, sentinel, g.vertices_count()) {}

   private:
    Simplex(G& g, vertex_type& s, vertex_type& t, const w_t& sentinel,
            size_t v_count)
        : m_tree(v_count) {
        m_tree[t] = g.add_edge(s, t, sentinel, sentinel, sentinel);
        add_to_tree(s);

        // std::cout << std::endl << std::endl;
        // print_representation(g, std::cout);
        // std::cout << tree_ << std::endl;

        for (decltype(m_tree.current_version()) old_version = 0;
             old_version != m_tree.current_version();) {
            old_version = m_tree.current_version();
            for (auto& v : g) {
                for (auto e = v.edges_begin(); e != v.edges_end(); ++e) {
                    auto link = e->edge().link();
                    if (link->cap_r_to(link->other(v)) > 0 &&
                        link->cap_r_to(v) == 0) {
                        auto cost = residual_cost(link, v);
                        if (cost < 0) {
                            auto lca = m_tree.find_lca(link);
                            auto old_link = m_tree.augment(link, lca);
                            // std::cout << std::endl
                            //     << tree_ << std::endl
                            //     << tree_.potentials() << std::endl
                            //     << "eligible: " << *link << ", r. cost: " <<
                            //     cost
                            //     << ", lca: " << *lca << ", old edge: " <<
                            //     *old_link << std::endl;
                            m_tree.replace(old_link, link, lca);
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
                auto& existing_l = m_tree[w];
                if (!existing_l) {
                    existing_l = l;
                    add_to_tree(w);
                }
            }
        }
    }
    auto residual_cost(link_type* link, vertex_type& v) {
        auto cost = link->cost() + m_tree.get_vertex_potential(link->target()) -
                    m_tree.get_vertex_potential(link->source());
        if (link->is_to(v)) cost *= -1;
        return cost;
    }
};
}  // namespace Network_flow_ns
}  // namespace Graph
