#pragma once

#include "adjacency_lists.h"
#include <map>

// todo rename the file

namespace Graph {

    namespace Network_flow_ns {

        template<typename V>
            class Flow_link {
                private:
                    using cap_type = typename V::edge_value_type;
                    V* const source_; // todo const?
                    V* const target_;
                    cap_type cap_;
                    cap_type flow_;
                public:
                    Flow_link(V* source, V* target, const cap_type& cap, const cap_type& flow)
                        :source_(source), target_(target), cap_(cap), flow_(flow)
                    {}

                    const V& source() const { return *source_; }
                    V& source() { return *source_; }
                    const V& target() const { return *target_; }
                    V& target() { return *target_; }

                    cap_type cap() const { return cap_; }
                    cap_type flow() const { return flow_; }

                    bool is_from(const V& v) const { return source() == v; }
                    bool is_to(const V& v) const { return target() == v; }

                    const V& other(const V& v) const { return v == source() ? target() : source(); }
                    V& other(V& v) { return v == source() ? target() : source(); }

                    int cap_r_to(const V& v) const { return is_from(v) ? flow_ : cap_ - flow_; }
                    void add_flow_r_to(const V& v, cap_type f) { flow_ += (is_from(v) ? -f : f); }
            };

        template<typename V>
            class Flow_edge {
                public:
                    using link_type = Flow_link<V>;
                private:
                    link_type* link_;
                public:
                    using value_type = typename V::edge_value_type;
                    Flow_edge(link_type* link) :link_(link) {}
                    const link_type* link() const { return link_; }
                    link_type* link() { return link_; }
                    void set_link(link_type* link) { link_ = link; }
            };

        template<typename V, typename C>
            class Flow;

        template<typename V, typename C>
            class Flow_vertex 
            : public Adjacency_lists_ns::Adj_lists_vertex_base
              <Graph_type::GRAPH, V, Flow_edge<Flow_vertex<V, C>>, Flow_vertex<V, C>>
        {
            private:
                using this_type = Flow_vertex<V, C>;
                friend class Adjacency_lists_ns::Adjacency_lists_base<Graph_type::GRAPH, this_type>;
                friend class Flow<V, C>;
                friend class Vector<this_type>;
                using Base = Adjacency_lists_ns::Adj_lists_vertex_base
                    <Graph_type::GRAPH, V, Flow_edge<this_type>, this_type>;
                using adj_lists_type = typename Base::adj_lists_type;

                Flow_vertex(V value, adj_lists_type* adjacency_lists)
                    :Base(value, adjacency_lists)
                {}
                Flow_vertex() :Base() {}
            public:
                using edge_type = typename Base::edge_type;
                using edge_value_type = C;
                ~Flow_vertex() {
                    for (auto e = Base::cedges_begin(); e != Base::cedges_end(); ++e) {
                        auto link = e->edge().link();
                        if (link->is_from(*this))
                            delete link;
                    }
                }
                void add_link(const Flow_vertex& v, const Flow_edge<Flow_vertex<V, C>>& edge) {
                    Base::links_.emplace_back(v.index(), edge);
                } // todo move to parent, remove FLOW specialization
        };

        template<typename V, typename C>
            class Flow : public Adjacency_lists_ns::Adjacency_lists_base<Graph_type::GRAPH, Flow_vertex<V, C>> {
                private:
                    using Base = Adjacency_lists_ns::Adjacency_lists_base<Graph_type::GRAPH, Flow_vertex<V, C>>;
                public:
                    using vertex_type = typename Base::vertex_type;
                    using link_type = Flow_link<vertex_type>;

                    Flow() = default;
                    Flow(const Flow& o) :Base(o, true) {
                        std::map<link_type*, link_type*> map;
                        for (auto& v : Base::vertices_) {
                            v.adjacency_lists_ = this;
                            for (auto e = v.edges_begin(); e != v.edges_end(); ++e) {
                                auto& edge = e->edge();
                                link_type* link = edge.link();
                                auto it = map.find(link);
                                link_type* new_link;
                                if (it == map.end()) {
                                    new_link = new link_type(
                                        &Base::vertex_at(link->source()), &Base::vertex_at(link->target()),
                                        link->cap(), link->flow()); 
                                    map.insert({link, new_link});
                                } else
                                    new_link = it->second;
                                edge.set_link(new_link);
                            }
                        }
                    }
                    Flow& operator=(const Flow& o) {
                        auto copy = o;
                        std::swap(*this, copy);
                        return *this;
                    }
                    Flow(Flow&&) = default;
                    Flow& operator=(Flow&&) = default;
                    Flow& add_edge(vertex_type& v, vertex_type& w, C cap, C flow) {
                        if (!v.link_exists(w)) {
                            auto link = new link_type(&v, &w, cap, flow);
                            v.add_link(w, link);
                            w.add_link(v, link);
                        }
                        return *this;
                    }
                private:
                    // todo implement, have to delete link
                    void remove_edge(vertex_type& v1, vertex_type& v2) {}
            };

        template<typename V, typename C>
            struct Internal_printer 
            : public Adjacency_lists_ns::Internal_printer_base<Flow<V, C>, Internal_printer<V, C>> {
                static void print_vertex(const Flow_vertex<V, C>& v, std::ostream& stream) {
                    for (auto e = v.cedges_begin(); e != v.cedges_end(); ++e) {
                        auto& link = *e->edge().link();
                        bool is_from = link.is_from(v);
                        stream << (is_from ? "->" : "<-") << link.other(v)
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
