#pragma once

#include <map>

#include "adjacency_lists.h"
#include "array_queue.h"
#include "graph.h"

namespace Graph {

namespace Network_flow_ns {

template <typename L, typename V, typename C = typename V::edge_value_type>
C cost_r_to(const L& link, const V& vertex) {
    return link.is_from(vertex) ? -link.cost() : link.cost();
}

class LinkEmptyBase {};

template <typename C>
class LinkCostBase {
   private:
    C m_cost;

   public:
    LinkCostBase(const C& cost) : m_cost(cost) {}
    C cost() const { return m_cost; }
};

template <typename V, typename B>
class FlowLink : public B {
   private:
    using cap_type = typename V::edge_value_type;
    V* const m_source;
    V* const m_target;
    cap_type m_cap;
    cap_type m_flow;

   public:
    using vertex_type = V;
    template <typename... Args>
    FlowLink(V* source, V* target, const cap_type& cap, const cap_type& flow,
             Args&&... args)
        : B(std::forward<Args>(args)...),
          m_source(source),
          m_target(target),
          m_cap(cap),
          m_flow(flow) {}

    const V& source() const { return *m_source; }
    V& source() { return *m_source; }
    const V& target() const { return *m_target; }
    V& target() { return *m_target; }

    cap_type cap() const { return m_cap; }
    cap_type flow() const { return m_flow; }

    bool is_from(const V& v) const { return source() == v; }
    bool is_to(const V& v) const { return target() == v; }

    const V& other(size_t index) const {
        return index == source() ? target() : source();
    }
    V& other(size_t index) { return index == source() ? target() : source(); }

    int cap_r_to(const V& v) const {
        return is_from(v) ? m_flow : m_cap - m_flow;
    }
    void add_flow_r_to(const V& v, cap_type f) {
        m_flow += (is_from(v) ? -f : f);
    }

    template <typename VV, typename BB>
    friend std::ostream& operator<<(std::ostream& stream,
                                    const FlowLink<VV, BB>& l);
};

template <typename VV, typename BB>
std::ostream& operator<<(std::ostream& stream, const FlowLink<VV, BB>& l) {
    return stream << l.source() << "-" << l.target();
}

template <typename V>
class FlowEdge {
   public:
    using link_type = FlowLink<V, typename V::link_base_type>;

   private:
    link_type* m_link;

   public:
    using value_type = typename V::edge_value_type;
    FlowEdge(link_type* link) : m_link(link) {}
    const link_type* link() const { return m_link; }
    link_type* link() { return m_link; }
    void set_link(link_type* link) { m_link = link; }
};

template <typename V>
class Flow;

template <typename V, typename C, typename LB = LinkEmptyBase>
class FlowVertex : public Adjacency_lists_ns::AdjListsVertexBase<
                       GraphType::GRAPH, V, FlowEdge<FlowVertex<V, C, LB>>,
                       FlowVertex<V, C, LB>> {
   private:
    using this_type = FlowVertex<V, C, LB>;
    friend class Adjacency_lists_ns::AdjacencyListsBase<GraphType::GRAPH,
                                                        this_type>;
    friend class Flow<this_type>;
    friend class Vector<this_type>;
    using Base =
        Adjacency_lists_ns::AdjListsVertexBase<GraphType::GRAPH, V,
                                               FlowEdge<this_type>, this_type>;
    using adj_lists_type = typename Base::adj_lists_type;
    FlowVertex(const V& value, adj_lists_type* adjacency_lists)
        : Base(value, adjacency_lists) {}
    FlowVertex() : Base() {}

    FlowVertex(const FlowVertex&) = default;
    FlowVertex& operator=(const FlowVertex&) = default;
    FlowVertex(FlowVertex&&) = default;
    FlowVertex& operator=(FlowVertex&&) = default;

   public:
    using edge_type = typename Base::edge_type;
    using link_base_type = LB;
    using edge_value_type = C;
    ~FlowVertex() {
        for (auto e = Base::cedges_begin(); e != Base::cedges_end(); ++e) {
            auto link = e->edge().link();
            if (link->is_from(*this)) delete link;
        }
    }
    void remove_edge(const FlowVertex& v) {
        Base::m_links.remove_first_if([&v](auto& edge) {
            bool found = v.index() == edge.target();
            if (found && v == edge.link()->source()) delete edge.link();
            return found;
        });
    }
};

template <typename V>
class Flow : public Adjacency_lists_ns::AdjacencyListsBase<GraphType::GRAPH, V>,
             public Adjacency_lists_ns::EdgesRemover<GraphType::GRAPH, V> {
   private:
    using Base = Adjacency_lists_ns::AdjacencyListsBase<GraphType::GRAPH, V>;

   public:
    using vertex_type = typename Base::vertex_type;
    using edge_value_type = typename vertex_type::edge_value_type;
    using link_base_type = typename vertex_type::link_base_type;
    using link_type = typename vertex_type::edge_type::link_type;

    Flow() = default;
    Flow(const Flow& o) : Base(o, true) {
        std::map<link_type*, link_type*> map;
        for (auto& v : Base::vertices_) {
            v.adjacency_lists_ = this;
            for (auto e = v.edges_begin(); e != v.edges_end(); ++e) {
                auto& edge = e->edge();
                link_type* link = edge.link();
                auto it = map.find(link);
                link_type* new_link;
                if (it == map.end()) {
                    new_link = new link_type(&Base::operator[](link->source()),
                                             &Base::operator[](link->target()),
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
    template <typename... Args>
    link_type* add_edge(vertex_type& v, vertex_type& w,
                        const edge_value_type& cap, const edge_value_type& flow,
                        Args&&... args) {
        if (!v.link_exists(w)) {
            auto link =
                new link_type(&v, &w, cap, flow, std::forward<Args>(args)...);
            v.add_link(w, link);
            w.add_link(v, link);
            return link;
        }
        return nullptr;
    }
    link_type* get_link(size_t v, size_t w) {
        return Base::get_edge(v, w)->link();
    }
};

template <typename V, typename C>
void print(const V& v, const FlowLink<V, LinkCostBase<C>>& link,
           std::ostream& stream) {
    stream << (link.is_from(v) ? "->" : "<-") << link.other(v).value() << "("
           << link.flow() << "/" << link.cap() << "[" << link.cost() << "]"
           << ") ";
}

template <typename V, typename LB>
void print(const V& v, const FlowLink<V, LB>& link, std::ostream& stream) {
    stream << (link.is_from(v) ? "->" : "<-") << link.other(v).value() << "("
           << link.flow() << "/" << link.cap() << ") ";
}

template <typename V, typename C, typename LB>
struct InternalPrinter
    : public Adjacency_lists_ns::InternalPrinterBase<
          Flow<FlowVertex<V, C, LB>>, InternalPrinter<V, C, LB>> {
    static void print_vertex(const FlowVertex<V, C, LB>& v,
                             std::ostream& stream) {
        for (auto e = v.cedges_begin(); e != v.cedges_end(); ++e)
            print(v, *e->edge().link(), stream);
    }
};

template <typename V, typename C, typename LB = Network_flow_ns::LinkEmptyBase>
void print_representation(
    const Network_flow_ns::Flow<Network_flow_ns::FlowVertex<V, C, LB>>& g,
    std::ostream& stream) {
    Network_flow_ns::InternalPrinter<V, C, LB>::print(g, stream);
}

template <typename G>
auto calculate_network_flow_cost(const G& g) {
    typename G::edge_type::value_type sum = 0;
    for (auto v = g.cbegin(); v != g.cend(); ++v)
        for (auto e = v->cedges_begin(); e != v->cedges_end(); ++e) {
            auto& link = *e->edge().link();
            if (!link.is_from(*v))
                sum += Network_flow_ns::cost_r_to(link, *v) * link.flow();
        }
    return sum;
}

template <typename G>
struct MaxFlow {
    using vertex_t = typename G::vertex_type;
    using w_t = typename G::edge_type::value_type;
    using edge_it_t = typename G::vertex_type::edges_iterator::entry_type;
    G& m_g;
    vertex_t& m_s;
    vertex_t& m_t;
    Array<w_t> m_weights;
    Array<typename G::link_type*> m_links;
    w_t m_sentinel;
    MaxFlow(G& g, vertex_t& s, vertex_t& t, w_t sentinel)
        : m_g(g),
          m_s(s),
          m_t(t),
          m_weights(g.vertices_count()),
          m_links(g.vertices_count()),
          m_sentinel(sentinel) {
        while (pfs()) augment();
    }
    bool pfs() {
        VertexHeap<vertex_t*, w_t> heap(m_g.vertices_count(), m_weights);
        for (auto v = m_g.cbegin(); v != m_g.cend(); ++v) {
            m_weights[*v] = 0;
            m_links[*v] = nullptr;
            heap.push(v);
        }
        m_weights[m_s] = -m_sentinel;
        heap.move_up(&m_s);

        while (!heap.empty()) {
            vertex_t& v = *heap.pop();

            m_weights[v] = -m_sentinel;
            if (v == m_t || (v != m_s && m_links[v] == nullptr)) break;

            for (auto e = v.edges_begin(); e != v.edges_end(); ++e) {
                auto link = e->edge().link();
                auto& w = link->other(v);
                auto cap = link->cap_r_to(w);
                auto p = cap < -m_weights[v] ? cap : -m_weights[v];
                if (cap > 0 && m_weights[w] > -p) {
                    m_weights[w] = -p;
                    heap.move_up(&w);
                    m_links[w] = e->edge().link();
                }
            }
        }
        return m_links[m_t] != nullptr;
    }
    vertex_t& other_vertex(vertex_t& v) { return m_links[v]->other(v); }
    void augment() {
        auto cap = m_links[m_t]->cap_r_to(m_t);
        for (vertex_t* v = &other_vertex(m_t); *v != m_s;) {
            auto link = m_links[*v];
            auto c = link->cap_r_to(*v);
            if (cap > c) cap = c;
            v = &link->other(*v);
        }
        m_links[m_t]->add_flow_r_to(m_t, cap);
        for (vertex_t* v = &other_vertex(m_t); *v != m_s; v = &other_vertex(*v))
            m_links[*v]->add_flow_r_to(*v, cap);
    }
};

template <typename G>
class PreFlowPushMaxFlow {
   private:
    using vertex_t = typename G::vertex_type;
    using w_t = typename G::edge_type::value_type;
    using edge_it_t = typename G::vertex_type::edges_iterator::entry_type;
    G& m_g;
    vertex_t& m_s;
    vertex_t& m_t;
    const size_t m_v_count;
    Array<size_t> m_heights;
    Array<w_t> m_weights;
    inline void init_heights() {}

   public:
    PreFlowPushMaxFlow(G& g, vertex_t& s, vertex_t& t, w_t sentinel)
        : m_g(g),
          m_s(s),
          m_t(t),
          m_v_count(g.vertices_count()),
          m_heights(m_v_count, m_v_count + 1),
          m_weights(m_v_count, 0) {
        ArrayQueue<vertex_t*> queue(m_v_count);
        queue.push(&m_t);
        m_heights[m_t] = 0;
        auto default_height = m_v_count + 1;
        while (!queue.empty()) {
            auto& v = *queue.pop();
            auto height = m_heights[v] + 1;
            for (auto e = v.edges_begin(); e != v.edges_end(); ++e) {
                auto link = e->edge().link();
                auto& w = link->other(v);
                if (m_heights[w] == default_height && link->is_from(w)) {
                    m_heights[w] = height;
                    queue.push(&w);
                }
            }
        }

        queue.push(&s);
        m_weights[t] = -(m_weights[s] = sentinel * m_v_count);

        while (!queue.empty()) {
            auto& v = *queue.pop();
            for (auto e = v.edges_begin(); e != v.edges_end(); ++e) {
                auto link = e->edge().link();
                auto& w = link->other(v);
                auto p = link->cap_r_to(w);
                if (p > m_weights[v]) p = m_weights[v];
                if ((p > 0 && v == s) || m_heights[v] == m_heights[w] + 1) {
                    link->add_flow_r_to(w, p);
                    m_weights[v] -= p;
                    m_weights[w] += p;
                    if (w != s && w != t) queue.push(&w);
                }
            }
            if (v != s && v != t && m_weights[v] > 0) {
                ++m_heights[v];
                queue.push(&v);
            }
        }
    }
};

template <typename F, typename M>
auto find_feasible_flow(F& f, const M& supply, const M& demand) {
    auto& s = f.create_vertex(-1);
    auto& t = f.create_vertex(-1);

    for (auto e = supply.cbegin(); e != supply.cend(); ++e)
        f.add_edge(s, f[e->first], e->second, 0);
    for (auto e = demand.cbegin(); e != demand.cend(); ++e)
        f.add_edge(f[e->first], t, e->second, 0);

    PreFlowPushMaxFlow m(f, s, t, f.vertices_count() * 10);
    return std::pair(&s, &t);
}
}  // namespace Network_flow_ns

template <typename V, typename C>
using NetworkFlow = Network_flow_ns::Flow<Network_flow_ns::FlowVertex<V, C>>;

template <typename V, typename C>
using NetworkFlowWithCost = Network_flow_ns::Flow<
    Network_flow_ns::FlowVertex<V, C, Network_flow_ns::LinkCostBase<C>>>;

namespace Network_flow_ns {

template <typename M>
auto bipartite_matching(const M& mapping) {
    using value_type = typename M::key_type;

    Builder<NetworkFlow<value_type, int>> b;
    for (auto e = mapping.cbegin(); e != mapping.cend(); ++e)
        b.for_vertex(e->first);
    for (auto e = mapping.cbegin(); e != mapping.cend(); ++e) {
        auto v = b.for_vertex(e->first);
        auto& list = e->second;
        for (auto t = list.cbegin(); t != list.cend(); ++t)
            v.add_edge(*t, 1, 0);
    }
    auto f = b.build();

    static const value_type default_value = value_type();
    auto& s = f.create_vertex(default_value);
    size_t i = 0;
    for (; i < mapping.size(); ++i) f.add_edge(s, f[i], 1, 0);

    auto& t = f.create_vertex(default_value);
    for (; i < f.vertices_count(); ++i) f.add_edge(f[i], t, 1, 0);

    MaxFlow m(f, s, t, f.vertices_count() * 10);

    std::map<value_type, value_type> result;
    for (i = 0; i < mapping.size(); ++i) {
        auto& v = f[i];
        for (auto e = v.cedges_begin(); e != v.cedges_end(); ++e) {
            auto& link = *e->edge().link();
            if (v == link.source() && link.flow() > 0) {
                result[v.value()] = link.other(v).value();
                break;
            }
        }
    }
    return result;
}

}  // namespace Network_flow_ns

}  // namespace Graph
