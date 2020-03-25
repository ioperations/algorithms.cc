#pragma once

namespace Graph {

    namespace Network_flow_ns {

        template<typename G>
            struct Max_flow_min_cost {
                using vertex_type = typename G::vertex_type;
                using link_type = typename vertex_type::edge_type::link_type;
                using w_t = typename vertex_type::edge_value_type;

                G& g_;
                Array<w_t> weights_;
                Array<link_type*> links_;
                const w_t sentinel_;

                Max_flow_min_cost(G& g, vertex_type& s, vertex_type& t, const w_t& sentinel)
                    :g_(g), weights_(g.vertices_count()), links_(g.vertices_count()), sentinel_(sentinel)
                {
                    g.add_edge(s, t, sentinel, sentinel, sentinel);
                    for (vertex_type* v; (v = find_negative_cycle()); )
                        augment(v);
                    g.remove_edge(s, t);
                }
                vertex_type* find_negative_cycle() {
                    for (auto& v : g_)
                        if (find_negative_cycle(v))
                            return &v;
                    return nullptr;
                }
                bool find_negative_cycle(vertex_type& vertex) {
                    weights_.fill(sentinel_);
                    weights_[vertex] = 0;

                    Forward_list<vertex_type*> queue;
                    queue.push_back(&vertex);
                    queue.push_back(nullptr);

                    size_t count = 0;
                    for (bool repeat = true; repeat && !queue.empty(); ) {
                        vertex_type* v;
                        while (repeat && (v = queue.pop_front()) == nullptr) {
                            repeat = ++count < g_.vertices_count();
                            if (repeat)
                                queue.push_back(nullptr);
                        }
                        if (repeat)
                            for (auto e = v->edges_begin(); e != v->edges_end(); ++e) {
                                auto link = e->edge().link();
                                auto& w = link->other(*v);
                                if (link->cap_r_to(w) > 0) {
                                    queue.push_back(&w);
                                    auto weight = weights_[*v] + cost_r_to(*link, w);
                                    if (weights_[w] > weight) {
                                        weights_[w] = weight;
                                        links_[w] = link;
                                    }
                                }
                            }
                    }
                    return weights_[vertex] < 0;
                }
                void augment(vertex_type* vertex) {
                    auto cap = sentinel_;
                    iterate_cycle(vertex, [&cap](auto link, auto v) {
                        cap = std::min(cap, link->cap_r_to(*v));
                    });
                    iterate_cycle(vertex, [cap](auto link, auto v) {
                        link->add_flow_r_to(*v, cap);
                    });
                }
                template<typename F>
                    void iterate_cycle(vertex_type* vertex, F f) {
                        for (auto v = vertex; ; ) {
                            auto link = links_[*v];
                            f(link, v);
                            v = &link->other(*v);
                            if (v == vertex) break;
                        }
                    }
            };

    }

}
