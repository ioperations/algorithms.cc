#pragma once

#include "adjacency_lists.h"

namespace Graph {

    namespace Network_flow_ns {

        template<typename V, typename T>
            class Flow_link {
                private:
                    V* const source_; // todo const?
                    V* const target_;
                    T cap_;
                    T flow_;
                public:
                    Flow_link(V* source, V* target, const T& cap, const T& flow)
                        :source_(source), target_(target), cap_(cap), flow_(flow)
                    {}
                    const V& source() const { return *source_; }
                    V& source() { return *source_; }
                    const V& target() const { return *target_; }
                    V& target() { return *target_; }
                    T cap() const { return cap_; }
                    T flow() const { return flow_; }
            };

        template<typename V, typename T>
            class Flow_edge {
                private:
                    using link_type = Flow_link<V, T>;
                    link_type* const link_;
                public:
                    using value_type = T;
                    Flow_edge(link_type* link) :link_(link) {}
                    const link_type* link() const { return link_; }
            };

        template<typename VT, typename ET>
            class Flow_vertex 
            : public Adjacency_lists_ns::Adj_lists_vertex_base<Graph_type::GRAPH, VT, Flow_edge<Flow_vertex<VT, ET>, ET>, Flow_vertex<VT, ET>>
            {
                private:
                    using Base = Adjacency_lists_ns::Adj_lists_vertex_base<Graph_type::GRAPH, VT, Flow_edge<Flow_vertex<VT, ET>, ET>, Flow_vertex<VT, ET>>;
                    using adj_lists_type = typename Base::adj_lists_type;
                public:
                    using edge_type = typename Base::edge_type;
                    Flow_vertex(VT value, adj_lists_type* adjacency_lists)
                        :Base(value, adjacency_lists)
                    {}
                    Flow_vertex() :Base() {}
                    ~Flow_vertex() {
                        for (auto e = Base::cedges_begin(); e != Base::cedges_end(); ++e) {
                            auto link = e->edge().link();
                            if (link->source() == *this)
                                delete link;
                        }
                    }
                    void add_link(const Flow_vertex& v, const Flow_edge<Flow_vertex<VT, ET>, ET>& edge) {
                        Base::links_.emplace_back(v.index(), edge);
                    } // todo move to parent, remove FLOW specialization
            };

        template<typename V, typename C>
            class Flow : public Adjacency_lists_ns::Adjacency_lists<Graph_type::GRAPH, Flow_vertex<V, C>> {
                private:
                    using Base = Adjacency_lists_ns::Adjacency_lists<Graph_type::GRAPH, Flow_vertex<V, C>>;
                public:
                    using vertex_type = typename Base::vertex_type;
                    void add_edge(vertex_type& v, vertex_type& w, C cap, C flow) {
                        auto link = new Flow_link<vertex_type, C>(&v, &w, cap, flow);
                        Base::add_edge(v, w, link);
                    }
            };

        template<typename V, typename C>
            struct Internal_printer 
            : public Adjacency_lists_ns::Internal_printer_base<Flow<V, C>, Internal_printer<V, C>> {
                static void print_vertex(const Flow_vertex<V, C>& v, std::ostream& stream) {
                    for (auto w = v.cedges_begin(); w != v.cedges_end(); ++w) {
                        auto& link = *w->edge().link();
                        stream << (link.source() == v ? "->" : "<-") << link.target()
                            << "(" << link.flow() << "/" << link.cap() << ") ";
                    }
                }
            };
    }

    template<typename V, typename C>
        using Network_flow = Network_flow_ns::Flow<V, C>;

    template<typename V, typename C>
        void print_representation(const Network_flow<V, C>& g, std::ostream& stream) {
            Network_flow_ns::Internal_printer<V, C>::print(g, stream);
        }

}
