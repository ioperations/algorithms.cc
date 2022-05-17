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

class Link_empty_base {};

template <typename C>
class Link_cost_base {
   private:
    C cost_;

   public:
    Link_cost_base(const C& cost) : cost_(cost) {}
    C cost() const { return cost_; }
};

template <typename V, typename B>
class Flow_link : public B {
   private:
    using cap_type = typename V::edge_value_type;
    V* const source_;
    V* const target_;
    cap_type cap_;
    cap_type flow_;

   public:
    using vertex_type = V;
    template <typename... Args>
    Flow_link(V* source, V* target, const cap_type& cap, const cap_type& flow,
              Args&&... args)
        : B(std::forward<Args>(args)...),
          source_(source),
          target_(target),
          cap_(cap),
          flow_(flow) {}

    const V& source() const { return *source_; }
    V& source() { return *source_; }
    const V& target() const { return *target_; }
    V& target() { return *target_; }

    cap_type cap() const { return cap_; }
    cap_type flow() const { return flow_; }

    bool is_from(const V& v) const { return source() == v; }
    bool is_to(const V& v) const { return target() == v; }

    const V& other(size_t index) const {
        return index == source() ? target() : source();
    }
    V& other(size_t index) { return index == source() ? target() : source(); }

    int cap_r_to(const V& v) const { return is_from(v) ? flow_ : cap_ - flow_; }
    void add_flow_r_to(const V& v, cap_type f) {
        flow_ += (is_from(v) ? -f : f);
    }

    template <typename VV, typename BB>
    friend std::ostream& operator<<(std::ostream& stream,
                                    const Flow_link<VV, BB>& l);
};

template <typename VV, typename BB>
std::ostream& operator<<(std::ostream& stream, const Flow_link<VV, BB>& l) {
    return stream << l.source() << "-" << l.target();
}

template <typename V>
class Flow_edge {
   public:
    using link_type = Flow_link<V, typename V::link_base_type>;

   private:
    link_type* link_;

   public:
    using value_type = typename V::edge_value_type;
    Flow_edge(link_type* link) : link_(link) {}
    const link_type* link() const { return link_; }
    link_type* link() { return link_; }
    void set_link(link_type* link) { link_ = link; }
};

template <typename V>
class Flow;

template <typename V, typename C, typename LB = Link_empty_base>
class Flow_vertex : public Adjacency_lists_ns::Adj_lists_vertex_base<
                        Graph_type::GRAPH, V, Flow_edge<Flow_vertex<V, C, LB>>,
                        Flow_vertex<V, C, LB>> {
   private:
    using this_type = Flow_vertex<V, C, LB>;
    friend class Adjacency_lists_ns::Adjacency_lists_base<Graph_type::GRAPH,
                                                          this_type>;
    friend class Flow<this_type>;
    friend class Vector<this_type>;
    using Base = Adjacency_lists_ns::Adj_lists_vertex_base<
        Graph_type::GRAPH, V, Flow_edge<this_type>, this_type>;
    using adj_lists_type = typename Base::adj_lists_type;
    Flow_vertex(const V& value, adj_lists_type* adjacency_lists)
        : Base(value, adjacency_lists) {}
    Flow_vertex() : Base() {}

    Flow_vertex(const Flow_vertex&) = default;
    Flow_vertex& operator=(const Flow_vertex&) = default;
    Flow_vertex(Flow_vertex&&) = default;
    Flow_vertex& operator=(Flow_vertex&&) = default;

   public:
    using edge_type = typename Base::edge_type;
    using link_base_type = LB;
    using edge_value_type = C;
    ~Flow_vertex() {
        for (auto e = Base::cedges_begin(); e != Base::cedges_end(); ++e) {
            auto link = e->edge().link();
            if (link->is_from(*this)) delete link;
        }
    }
    void remove_edge(const Flow_vertex& v) {
        Base::links_.remove_first_if([&v](auto& edge) {
            bool found = v.index() == edge.target();
            if (found && v == edge.link()->source()) delete edge.link();
            return found;
        });
    }
};

template <typename V>
class Flow
    : public Adjacency_lists_ns::Adjacency_lists_base<Graph_type::GRAPH, V>,
      public Adjacency_lists_ns::Edges_remover<Graph_type::GRAPH, V> {
   private:
    using Base = Adjacency_lists_ns::Adjacency_lists_base<Graph_type::GRAPH, V>;

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
void print(const V& v, const Flow_link<V, Link_cost_base<C>>& link,
           std::ostream& stream) {
    stream << (link.is_from(v) ? "->" : "<-") << link.other(v).value() << "("
           << link.flow() << "/" << link.cap() << "[" << link.cost() << "]"
           << ") ";
}

template <typename V, typename LB>
void print(const V& v, const Flow_link<V, LB>& link, std::ostream& stream) {
    stream << (link.is_from(v) ? "->" : "<-") << link.other(v).value() << "("
           << link.flow() << "/" << link.cap() << ") ";
}

template <typename V, typename C, typename LB>
struct Internal_printer
    : public Adjacency_lists_ns::Internal_printer_base<
          Flow<Flow_vertex<V, C, LB>>, Internal_printer<V, C, LB>> {
    static void print_vertex(const Flow_vertex<V, C, LB>& v,
                             std::ostream& stream) {
        for (auto e = v.cedges_begin(); e != v.cedges_end(); ++e)
            print(v, *e->edge().link(), stream);
    }
};

template <typename V, typename C,
          typename LB = Network_flow_ns::Link_empty_base>
void print_representation(
    const Network_flow_ns::Flow<Network_flow_ns::Flow_vertex<V, C, LB>>& g,
    std::ostream& stream) {
    Network_flow_ns::Internal_printer<V, C, LB>::print(g, stream);
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
struct Max_flow {
    using vertex_t = typename G::vertex_type;
    using w_t = typename G::edge_type::value_type;
    using edge_it_t = typename G::vertex_type::edges_iterator::entry_type;
    G& g_;
    vertex_t& s_;
    vertex_t& t_;
    Array<w_t> weights_;
    Array<typename G::link_type*> links_;
    w_t sentinel_;
    Max_flow(G& g, vertex_t& s, vertex_t& t, w_t sentinel)
        : g_(g),
          s_(s),
          t_(t),
          weights_(g.vertices_count()),
          links_(g.vertices_count()),
          sentinel_(sentinel) {
        while (pfs()) augment();
    }
    bool pfs() {
        Vertex_heap<vertex_t*, w_t> heap(g_.vertices_count(), weights_);
        for (auto v = g_.cbegin(); v != g_.cend(); ++v) {
            weights_[*v] = 0;
            links_[*v] = nullptr;
            heap.push(v);
        }
        weights_[s_] = -sentinel_;
        heap.move_up(&s_);

        while (!heap.empty()) {
            vertex_t& v = *heap.pop();

            weights_[v] = -sentinel_;
            if (v == t_ || (v != s_ && links_[v] == nullptr)) break;

            for (auto e = v.edges_begin(); e != v.edges_end(); ++e) {
                auto link = e->edge().link();
                auto& w = link->other(v);
                auto cap = link->cap_r_to(w);
                auto p = cap < -weights_[v] ? cap : -weights_[v];
                if (cap > 0 && weights_[w] > -p) {
                    weights_[w] = -p;
                    heap.move_up(&w);
                    links_[w] = e->edge().link();
                }
            }
        }
        return links_[t_] != nullptr;
    }
    vertex_t& other_vertex(vertex_t& v) { return links_[v]->other(v); }
    void augment() {
        auto cap = links_[t_]->cap_r_to(t_);
        for (vertex_t* v = &other_vertex(t_); *v != s_;) {
            auto link = links_[*v];
            auto c = link->cap_r_to(*v);
            if (cap > c) cap = c;
            v = &link->other(*v);
        }
        links_[t_]->add_flow_r_to(t_, cap);
        for (vertex_t* v = &other_vertex(t_); *v != s_; v = &other_vertex(*v))
            links_[*v]->add_flow_r_to(*v, cap);
    }
};

template <typename G>
class Pre_flow_push_max_flow {
   private:
    using vertex_t = typename G::vertex_type;
    using w_t = typename G::edge_type::value_type;
    using edge_it_t = typename G::vertex_type::edges_iterator::entry_type;
    G& g_;
    vertex_t& s_;
    vertex_t& t_;
    const size_t v_count_;
    Array<size_t> heights_;
    Array<w_t> weights_;
    inline void init_heights() {}

   public:
    Pre_flow_push_max_flow(G& g, vertex_t& s, vertex_t& t, w_t sentinel)
        : g_(g),
          s_(s),
          t_(t),
          v_count_(g.vertices_count()),
          heights_(v_count_, v_count_ + 1),
          weights_(v_count_, 0) {
        Array_queue<vertex_t*> queue(v_count_);
        queue.push(&t_);
        heights_[t_] = 0;
        auto default_height = v_count_ + 1;
        while (!queue.empty()) {
            auto& v = *queue.pop();
            auto height = heights_[v] + 1;
            for (auto e = v.edges_begin(); e != v.edges_end(); ++e) {
                auto link = e->edge().link();
                auto& w = link->other(v);
                if (heights_[w] == default_height && link->is_from(w)) {
                    heights_[w] = height;
                    queue.push(&w);
                }
            }
        }

        queue.push(&s);
        weights_[t] = -(weights_[s] = sentinel * v_count_);

        while (!queue.empty()) {
            auto& v = *queue.pop();
            for (auto e = v.edges_begin(); e != v.edges_end(); ++e) {
                auto link = e->edge().link();
                auto& w = link->other(v);
                auto p = link->cap_r_to(w);
                if (p > weights_[v]) p = weights_[v];
                if ((p > 0 && v == s) || heights_[v] == heights_[w] + 1) {
                    link->add_flow_r_to(w, p);
                    weights_[v] -= p;
                    weights_[w] += p;
                    if (w != s && w != t) queue.push(&w);
                }
            }
            if (v != s && v != t && weights_[v] > 0) {
                ++heights_[v];
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

    Pre_flow_push_max_flow m(f, s, t, f.vertices_count() * 10);
    return std::pair(&s, &t);
}
}  // namespace Network_flow_ns

template <typename V, typename C>
using Network_flow = Network_flow_ns::Flow<Network_flow_ns::Flow_vertex<V, C>>;

template <typename V, typename C>
using Network_flow_with_cost = Network_flow_ns::Flow<
    Network_flow_ns::Flow_vertex<V, C, Network_flow_ns::Link_cost_base<C>>>;

namespace Network_flow_ns {

template <typename M>
auto bipartite_matching(const M& mapping) {
    using value_type = typename M::key_type;

    Builder<Network_flow<value_type, int>> b;
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

    Max_flow m(f, s, t, f.vertices_count() * 10);

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
