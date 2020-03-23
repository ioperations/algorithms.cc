#pragma once

namespace Graph {

    namespace Network_flow_ns {

        template<typename G>
            struct Cycle {
                using link_type = typename G::link_type;
                using vertex_type = typename G::vertex_type; 
                struct Iterator {
                    private:
                        vertex_type* v_;
                        Array<link_type*>& links_;
                        size_t index_;
                    public:
                        Iterator(vertex_type* v, Array<link_type*>& links, size_t index)
                            :v_(v), links_(links), index_(index)
                        {}
                        vertex_type& source() { return *v_; }
                        link_type* link() { return links_[index_ - 1]; }
                        Iterator& operator++() {
                            v_ = &link()->other(*v_);
                            --index_;
                            return *this;
                        }
                        bool has_next() { return index_ > 0; }
                };

                bool exists_;
                Array<link_type*> cycle_;
                size_t size_;
                vertex_type* first_v_;

                Cycle(G& g, Array<link_type*>&& spt)
                    :exists_(false), cycle_(g.vertices_count()), size_(0)
                {
                    for (size_t i = spt.size(); !exists_ && i > 0; --i) {
                        auto& l = spt[i - 1];
                        if (l) {
                            auto current_link = l;
                            auto t = &g.vertex_at(i - i);
                            auto s = &l->other(*t);
                            first_v_ = s;
                            for (size_t j = 0; !exists_ && j < g.vertices_count(); ++j) {
                                current_link = spt[*s];
                                if (current_link) {
                                    cycle_[size_++] = current_link;
                                    s = &current_link->other(*s);
                                    exists_ = *s == *first_v_;
                                }
                            }
                        }
                    }
                }
                bool exists() { return exists_; }
                Iterator iterator() { return {first_v_, cycle_, size_}; }
            };

        template<typename G>
            struct Spt {
                using vertex_type = typename G::vertex_type;
                using w_t = typename G::edge_type::value_type;
                using link_type = typename G::link_type;

                Array<typename G::edge_type::value_type> weights_;
                Array<link_type*> spt_;

                Spt(G& g, vertex_type& s, const w_t sentinel)
                    :weights_(g.vertices_count(), sentinel), spt_(g.vertices_count(), nullptr)
                {

                    weights_[s] = 0;

                    Forward_list<vertex_type*> queue;
                    queue.push_back(&s);
                    queue.push_back(nullptr);

                    size_t n = 0;

                    bool completed = false;
                    while (!completed && !queue.empty()) {
                        vertex_type* v;
                        while (!completed && (v = queue.pop_front()) == nullptr) {
                            completed = n++ > g.vertices_count();
                            if (!completed)
                                queue.push_back(nullptr);
                        }
                        if (!completed)
                            for (auto e = v->edges_begin(); e != v->edges_end(); ++e) {
                                auto& link = *e->edge().link();
                                auto& w = link.other(*v);

                                bool loop_occurred = false;
                                auto p_link = spt_[*v];
                                if (p_link)
                                    loop_occurred = p_link->source() == w || p_link->target() == w;

                                if (!loop_occurred) {
                                    auto cap = link.cap_r_to(w);
                                    if (cap > 0) {
                                        auto cost = cost_r_to(link, w) * cap;
                                        auto weight = weights_[*v] + cost;
                                        if (weights_[w] > weight) {
                                            weights_[w] = weight;
                                            queue.push_back(&w);
                                            spt_[w] = e->edge().link();
                                        }
                                    }
                                }
                            }
                    }

                    // std::cout << spt_ << std::endl;

                    // for (auto l : spt_)
                    //     if (l)
                    //     std::cout << l->source() << " - " << l->target() << std::endl;
                }
            };

        template<typename G>
            auto find_cycle(G& g, typename G::vertex_type& s, const typename G::edge_type::value_type sentinel) {
                Spt spt11(g, s, sentinel);
                return Cycle(g, std::move(spt11.spt_));
            }

        template<typename G>
            void minimize_network_flow_cost(G& g, typename G::vertex_type& s, typename G::vertex_type& t,
                                            const typename G::edge_type::value_type sentinel) {
                std::cout << std::endl;
                print_representation(g, std::cout);

                struct Foo {
                    using vertex_type = typename G::vertex_type;
                    using w_t = typename G::edge_type::value_type;
                    G& g_;
                    vertex_type& s_;
                    const w_t sentinel_;
                    Foo(G& g, vertex_type& s, const w_t sentinel)
                        :g_(g), s_(s), sentinel_(sentinel)
                    {}

                    void augment(Cycle<G>& cycle) {
                        for (auto it2 = cycle.iterator(); it2.has_next(); ++it2) {
                            auto link = it2.link();
                            std::cout << it2.source() << " - " << link->other(it2.source()) << ": ";
                            std::cout << cost_r_to(*link, s_) * link->flow() << std::endl;
                        }

                        auto it = cycle.iterator();
                        auto cap = it.link()->cap_r_to(it.source());
                        for (++it; it.has_next(); ++it)
                            cap = std::min(cap, it.link()->cap_r_to(it.source()));

                        std::cout << "cap : " << cap << std::endl;

                        for (auto it2 = cycle.iterator(); it2.has_next(); ++it2) {
                            it2.link()->add_flow_r_to(it2.source(), -cap);
                        }
                        for (auto it2 = cycle.iterator(); it2.has_next(); ++it2) {
                            auto link = it2.link();
                            std::cout << it2.source() << " - " << link->other(it2.source()) << ": ";
                            std::cout << cost_r_to(*link, s_) * link->flow() << std::endl;
                        }

                        std::cout << "network flow cost: " << calculate_network_flow_cost(g_) << std::endl;

                    }

                    void bar() {
                        auto cycle = find_cycle(g_, g_.vertex_at(0), sentinel_);
                        if (cycle.exists()) {
                            augment(cycle);
                            print_representation(g_, std::cout);
                        }

                        {

                            Spt spt(g_, g_.vertex_at(3), sentinel_);
                            std::cout << spt.weights_ << std::endl;

                            for (auto e : spt.spt_) {
                                if (e)
                                    std::cout << e->source() << " " << e->target() << std::endl;
                            }

                            // auto cycle = find_cycle(g_, g_.vertex_at(3), sentinel_);
                            // if (cycle.exists()) {
                            //     for (auto it2 = cycle.iterator(); it2.has_next(); ++it2) {
                            //         auto link = it2.link();
                            //         auto& s = it2.source();
                            //         auto& t = it2.link()->other(s);
                            //         std::cout << s << " - " << t << ": ";
                            //         std::cout << cost_r_to(*link, s_) * link->flow() << std::endl;
                            //     }
                            // }
                        }


                    }
                };

                std::cout << "network flow cost: " << calculate_network_flow_cost(g) << std::endl;
                Foo foo(g, s, sentinel);
                foo.bar();



            }

    }

}
