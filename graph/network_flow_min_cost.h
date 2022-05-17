#pragma once

#include "array.h"
#include "forward_list"
namespace Graph {

namespace Network_flow_ns {

template <typename G>
struct MaxFlowMinCost {
    using vertex_type = typename G::vertex_type;
    using link_type = typename vertex_type::edge_type::link_type;
    using w_t = typename vertex_type::edge_value_type;

    G& m_g;
    Array<w_t> m_weights;
    Array<link_type*> m_links;
    const w_t m_sentinel;

    MaxFlowMinCost(G& g, vertex_type& s, vertex_type& t, const w_t& sentinel)
        : m_g(g),
          m_weights(g.vertices_count()),
          m_links(g.vertices_count()),
          m_sentinel(sentinel) {
        g.add_edge(s, t, sentinel, sentinel, sentinel);
        for (vertex_type* v; (v = find_negative_cycle());) augment(v);
        g.remove_edge(s, t);
    }
    vertex_type* find_negative_cycle() {
        for (auto& v : m_g)
            if (find_negative_cycle(v)) return &v;
        return nullptr;
    }
    bool find_negative_cycle(vertex_type& vertex) {
        m_weights.fill(m_sentinel);
        m_weights[vertex] = 0;

        Forward_list<vertex_type*> queue;
        queue.push_back(&vertex);
        queue.push_back(nullptr);

        size_t count = 0;
        for (bool repeat = true; repeat && !queue.empty();) {
            vertex_type* v;
            while (repeat && (v = queue.pop_front()) == nullptr) {
                repeat = ++count < m_g.vertices_count();
                if (repeat) queue.push_back(nullptr);
            }
            if (repeat)
                for (auto e = v->edges_begin(); e != v->edges_end(); ++e) {
                    auto link = e->edge().link();
                    auto& w = link->other(*v);
                    if (link->cap_r_to(w) > 0) {
                        queue.push_back(&w);
                        auto weight = m_weights[*v] + cost_r_to(*link, w);
                        if (m_weights[w] > weight) {
                            m_weights[w] = weight;
                            m_links[w] = link;
                        }
                    }
                }
        }
        return m_weights[vertex] < 0;
    }
    void augment(vertex_type* vertex) {
        auto cap = m_sentinel;
        iterate_cycle(vertex, [&cap](auto link, auto v) {
            cap = std::min(cap, link->cap_r_to(*v));
        });
        iterate_cycle(
            vertex, [cap](auto link, auto v) { link->add_flow_r_to(*v, cap); });
    }
    template <typename F>
    void iterate_cycle(vertex_type* vertex, F f) {
        for (auto v = vertex;;) {
            auto link = m_links[*v];
            f(link, v);
            v = &link->other(*v);
            if (v == vertex) break;
        }
    }
};

}  // namespace Network_flow_ns

}  // namespace Graph
