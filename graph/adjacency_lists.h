#pragma once

#include <iostream>

#include "graph_common.h"
#include "vector.h"
#include "forward_list.h"

namespace Graph {

    namespace Adjacency_lists_ns {

        template<typename T>
            class Edge {
                protected:
                    T weight_;
                public:
                    using value_type = T;
                    Edge(const T& t) :weight_(t) {}
                    T weight() const { return weight_; }
                    void set_weight(const T& weight) { weight_ = weight; }
            };

        template<typename T>
            class Flow_edge : public Edge<T> {
                private:
                   T flow_;
                public:
                   Flow_edge(const T& cap, const T& flow) :Edge<T>(cap), flow_(flow) {}
                   T cap() const { return Edge<T>::weight_; }
                   T flow() const { return flow_; }
                   void set_flow(const T& flow) { flow_ = flow; }
            };

        class Vertex_link_base {
            private:
                size_t target_;
            public:
                Vertex_link_base(size_t target) :target_(target) {}
                size_t target() const { return target_; }
        };

        template<Graph_type T_graph_type, typename E>
            class Vertex_link : public Vertex_link_base, public E {
                public:
                    Vertex_link(size_t target, const E& edge) :Vertex_link_base(target), E(edge) {}
            };

        template<Graph_type T_graph_type>
            class Vertex_link<T_graph_type, Edge<bool>> : public Vertex_link_base {
                public:
                    using value_type = bool;
                    Vertex_link(size_t target, const Edge<bool>& edge) :Vertex_link_base(target) {}
            };

        template<typename E>
            class Vertex_link<Graph_type::FLOW, E> : public Vertex_link_base, public E {
                private:
                    bool out_;
                public:
                    Vertex_link(size_t target, const E& edge, bool out) 
                        :Vertex_link_base(target), E(edge), out_(out) 
                    {}
                    bool is_out() const { return out_; }
            };

        template<Graph_type TT_graph_type, typename VV, typename EE>
            class Adjacency_lists_base;

        template<Graph_type T_graph_type, typename V, typename E, typename D>
            class Adj_lists_vertex_base : public Vertex_base<V> {
                private:
                    template<bool T_is_const>
                        class Iterator;
                    template<bool T_is_const>
                        class Edges_iterator;
                    template<Graph_type TT_graph_type, typename VV, typename EE, typename EET>
                        friend class Adjacency_lists;
                    using vertex_link_type = Vertex_link<T_graph_type, E>;

                    inline D* derived() { return static_cast<D*>(this); }
                    inline const D* derived() const { return static_cast<const D*>(this); }
                protected:
                    using adj_lists_type = Adjacency_lists_base<T_graph_type, D, E>;
                    adj_lists_type* adjacency_lists_;
                    Forward_list<vertex_link_type> links_;

                    Adj_lists_vertex_base(V value, adj_lists_type* adjacency_lists)
                        :Vertex_base<V>(value), adjacency_lists_(adjacency_lists)
                    {}
                    Adj_lists_vertex_base() :adjacency_lists_(nullptr) {}

                    bool link_exists(const Adj_lists_vertex_base& v) {
                        for (auto link = links_.begin(); link != links_.end(); ++link)
                            if(link->target() == v.index())
                                return true;
                        return false;
                    }
                public:
                    using iterator = Iterator<false>;
                    using const_iterator = Iterator<true>;
                    using edges_iterator = Edges_iterator<false>;
                    using const_edges_iterator = Edges_iterator<true>;

                    size_t index() const { 
                        return derived() - adjacency_lists_->vertices_.cbegin(); 
                    }
                    operator size_t() const { return index(); }

                    iterator begin() { return {derived(), links_.begin()}; }
                    iterator end() { return {derived(), links_.end()}; }
                    const_iterator cbegin() const { return {derived(), links_.cbegin()}; }
                    const_iterator cend() const { return {derived(), links_.cend()}; }

                    edges_iterator edges_begin() { return {derived(), links_.begin()}; }
                    edges_iterator edges_end() { return {derived(), links_.end()}; }
                    const_edges_iterator cedges_begin() const { return {derived(), links_.cbegin()}; }
                    const_edges_iterator cedges_end() const { return {derived(), links_.cend()}; }

                    void remove_edge(const Adj_lists_vertex_base& v) {
                        links_.remove_first_if([&v](const vertex_link_type& edge) { return v.index() == edge.target(); });
                    }
                    bool has_edge(const Adj_lists_vertex_base& v) const {
                        for (auto l = cbegin(); l != cend(); ++l)
                            if (*l == v)
                                return true;
                        return false;
                    }
                    const E* get_edge(const Adj_lists_vertex_base& v) const {
                        for (auto e = cedges_begin(); e != cedges_end(); ++e)
                            if (e->target() == v)
                                return e->edge_;
                        return nullptr;
                    }
                    E* get_edge(const Adj_lists_vertex_base& v) {
                        for (auto e = edges_begin(); e != edges_end(); ++e)
                            if (e->target() == v)
                                return e->edge_;
                        return nullptr;
                    }
            };

        template<Graph_type T_graph_type, typename V, typename E> // todo remove parent?
            class Vertex : public Adj_lists_vertex_base<T_graph_type, V, E, Vertex<T_graph_type, V, E>> {
                private:
                    using Base = Adj_lists_vertex_base<T_graph_type, V, E, Vertex<T_graph_type, V, E>>;
                    using adj_lists_type = typename Base::adj_lists_type;
                    template<Graph_type TT_graph_type, typename VV, typename EE>
                        friend class Adjacency_lists_base;
                    template<Graph_type TT_graph_type, typename VV, typename EE, typename EET>
                        friend class Adjacency_lists;
                    friend class Vector<Vertex>;
                    void add_link(const Vertex& v, const E& edge) {
                        Base::links_.emplace_back(v.index(), edge);
                    }
                public: // todo move to private?
                    Vertex(V value, adj_lists_type* adjacency_lists) // todo pass value by ref
                        :Base(value, adjacency_lists)
                    {}
                    Vertex() :Base() {}

            };

        template<typename V, typename E>
            class Vertex<Graph_type::FLOW, V, E> 
            : public Adj_lists_vertex_base<Graph_type::FLOW, V, E, Vertex<Graph_type::FLOW, V, E>> {
                private:
                    using Base = Adj_lists_vertex_base<Graph_type::FLOW, V, E, Vertex<Graph_type::FLOW, V, E>>;
                    using adj_lists_type = typename Base::adj_lists_type;
                    template<Graph_type TT_graph_type, typename VV, typename EE>
                        friend class Adjacency_lists_base;
                    template<Graph_type TT_graph_type, typename VV, typename EE, typename EET>
                        friend class Adjacency_lists;
                    friend class Vector<Vertex>;
                    Vertex(V value, adj_lists_type* adjacency_lists)
                        :Base(value, adjacency_lists)
                    {}
                    Vertex() :Base() {}

                    void add_out_link(const Vertex& v, const E& edge) {
                        Base::links_.emplace_back(v.index(), edge, true);
                    }
                    void add_in_link(const Vertex& v, const E& edge) {
                        Base::links_.emplace_back(v.index(), edge, false);
                    }
            };

        template<Graph_type T_graph_type, typename V, typename E>
            class Adjacency_lists_base {
                public:
                    using vertex_type = V;
                private:
                    template<Graph_type TT_graph_type, typename VV, typename EE, typename D>
                        friend class Adj_lists_vertex_base;
                    Vector<vertex_type> vertices_;
                    void update_vertices_this_link() {
                        for (auto& v : vertices_)
                            v.adjacency_lists_ = this;
                    }
                    template<Graph_type TT_graph_type, typename VV, typename EE>
                        struct Internal_printer {
                            static void print(const Vertex<TT_graph_type, VV, EE>& v, std::ostream& stream) {
                                for (auto w = v.cedges_begin(); w != v.cedges_end(); ++w)
                                    stream << w->target().index() << "(" << w->edge().weight() << ") ";
                            }
                        };
                    template<Graph_type TT_graph_type, typename VV>
                        struct Internal_printer<TT_graph_type, VV, Edge<bool>> {
                            static void print(const Vertex<TT_graph_type, VV, Edge<bool>>& v, std::ostream& stream) {
                                for (auto w = v.cbegin(); w != v.cend(); ++w)
                                    stream << w->index() << " ";
                            }
                        };
                    template<typename VV, typename EE>
                        struct Internal_printer<Graph_type::FLOW, VV, EE> {
                            static void print(const Vertex<Graph_type::FLOW, VV, EE>& v, std::ostream& stream) {
                                for (auto w = v.cedges_begin(); w != v.cedges_end(); ++w) {
                                    auto& e = w->edge();
                                    stream << (e.is_out() ? "->" : "<-") << w->target().index() 
                                        << "(" << e.weight() << ") ";
                                }
                            }
                        };
                public:
                    // using value_type = V;
                    using edge_type = E;
                    Adjacency_lists_base() = default;
                    Adjacency_lists_base(const Adjacency_lists_base& o) :vertices_(o.vertices_) {
                        update_vertices_this_link();
                    }
                    Adjacency_lists_base& operator=(const Adjacency_lists_base& o) {
                        auto copy = o;
                        std::swap(*this, copy);
                        return *this;
                    }
                    Adjacency_lists_base(Adjacency_lists_base&& o) :vertices_(std::move(o.vertices_)) {
                        update_vertices_this_link();
                    }
                    Adjacency_lists_base& operator=(Adjacency_lists_base&& o) {
                        std::swap(vertices_, o.vertices_);
                        update_vertices_this_link();
                        return *this;
                    }

                    vertex_type& create_vertex(const typename V::value_type& t) {
                        vertices_.push_back(vertex_type(t, this)); // todo emplace back?
                        return vertices_[vertices_.size() - 1];
                    }

                    size_t vertices_count() const { return vertices_.size(); }
                    const vertex_type& vertex_at(size_t index) const { return vertices_[index]; }
                    vertex_type& vertex_at(size_t index) { return vertices_[index]; }

                    bool has_edge(const vertex_type& v, const vertex_type& w) const { return v.has_edge(w); }

                    const E* get_edge(const vertex_type& v, const vertex_type& w) const { return v.get_edge(w); }
                    E* get_edge(vertex_type& v, const vertex_type& w) { return v.get_edge(w); }

                    auto cbegin() const { return vertices_.cbegin(); }
                    auto cend() const { return vertices_.cend(); }
                    auto begin() { return vertices_.begin(); }
                    auto end() { return vertices_.end(); } 

                    auto crbegin() const { return vertices_.crbegin(); }
                    auto crend() const { return vertices_.crend(); }

                    void print_internal(std::ostream& stream) {
                        for (auto& v : vertices_) {
                            stream << v.index() << ": ";
                            Internal_printer<T_graph_type, typename V::value_type, E>::print(v, stream);
                            stream << std::endl;
                        }
                    }
            };

        template<Graph_type T_graph_type, typename V, typename E>
            struct Edges_remover {
                using vertex_type = V;
                // using vertex_type = typename Adjacency_lists_base<T_graph_type, V, E>::vertex_type;
                void remove_edge(vertex_type& v1, vertex_type& v2) {
                    v1.remove_edge(v2);
                    v2.remove_edge(v1);
                }
            };

        template<typename V, typename E>
            struct Edges_remover<Graph_type::DIGRAPH, V, E> {
                using vertex_type = V;
                // using vertex_type = typename Adjacency_lists_base<Graph_type::DIGRAPH, V, E>::vertex_type;
                void remove_edge(vertex_type& v1, vertex_type& v2) {
                    v1.remove_edge(v2);
                }
            };

        template<Graph_type T_graph_type, typename V, typename E, typename ET = typename E::value_type>
            class Adjacency_lists : public Adjacency_lists_base<T_graph_type, V, E>, public Edges_remover<T_graph_type, V, E> {
                public:
                    using vertex_type = V;
                    // using vertex_type = typename Adjacency_lists_base<T_graph_type, V, E>::vertex_type;
                    Adjacency_lists& add_edge(vertex_type& v1, vertex_type& v2, const E& edge) {
                        if (!v1.link_exists(v2)) {
                            v1.add_link(v2, edge);
                            v2.add_link(v1, edge);
                        }
                        return *this;
                    }
            };

        template<typename V, typename E>
            class Adjacency_lists<Graph_type::GRAPH, V, E, bool>
            : public Adjacency_lists_base<Graph_type::GRAPH, V, E>, public Edges_remover<Graph_type::GRAPH, V, E> {
                public:
                    using vertex_type = V;
                    // using vertex_type = typename Adjacency_lists_base<Graph_type::GRAPH, V, E>::vertex_type;
                    Adjacency_lists& add_edge(vertex_type& v1, vertex_type& v2) {
                        if (!v1.link_exists(v2)) {
                            v1.add_link(v2, true);
                            v2.add_link(v1, true);
                        }
                        return *this;
                    }
            };

        template<typename V, typename E, typename ET>
            class Adjacency_lists<Graph_type::DIGRAPH, V, E, ET> 
            : public Adjacency_lists_base<Graph_type::DIGRAPH, V, E>, public Edges_remover<Graph_type::DIGRAPH, V, E> {
                public:
                    using vertex_type = V;
                    // using vertex_type = typename Adjacency_lists_base<Graph_type::DIGRAPH, V, E>::vertex_type;
                    Adjacency_lists& add_edge(vertex_type& v1, vertex_type& v2, const E& edge) {
                        if (!v1.link_exists(v2))
                            v1.add_link(v2, edge);
                        return *this;
                    }
            };

        template<typename V, typename E>
            class Adjacency_lists<Graph_type::DIGRAPH, V, E, bool>
            : public Adjacency_lists_base<Graph_type::DIGRAPH, V, E>, public Edges_remover<Graph_type::DIGRAPH, V, E> {
                public:
                    using vertex_type = V;
                    // using vertex_type = typename Adjacency_lists_base<Graph_type::DIGRAPH, V, E>::vertex_type;
                    Adjacency_lists& add_edge(vertex_type& v1, vertex_type& v2) {
                        if (!v1.link_exists(v2))
                            v1.add_link(v2, true);
                        return *this;
                    }
            };

        template<typename V, typename E, typename ET>
            class Adjacency_lists<Graph_type::FLOW, V, E, ET> 
            : public Adjacency_lists_base<Graph_type::FLOW, V, E>, public Edges_remover<Graph_type::FLOW, V, E> {
                public:
                    using vertex_type = V;
                    // using vertex_type = typename Adjacency_lists_base<Graph_type::FLOW, V, E>::vertex_type;
                    Adjacency_lists& add_edge(vertex_type& v1, vertex_type& v2, const E& edge) {
                        if (!v1.link_exists(v2)) {
                            v1.add_out_link(v2, edge);
                            v2.add_in_link(v1, edge);
                        }
                        return *this;
                    }
            };
 
        // todo rename V to VT
        template<Graph_type T_graph_type, typename V, typename E, typename D>
            template<bool T_is_const>
            class Adj_lists_vertex_base<T_graph_type, V, E, D>::Iterator {
                private:
                    using value_type = std::conditional_t<T_is_const, const D, D>;
                protected:
                    using link_type = Vertex_link<T_graph_type, E>;
                    using links_type = Forward_list<link_type>;
                    using links_iterator_type = std::conditional_t<T_is_const,
                          typename links_type::const_iterator, typename links_type::iterator>;
                    value_type* vertex_;
                private:
                    friend class Adj_lists_vertex_base;
                    links_iterator_type it_;
                    Iterator(value_type* vertex, const links_iterator_type& it)
                        :vertex_(vertex), it_(it)
                    {}
                public:
                    const Iterator& operator++() {
                        ++it_;
                        return *this;
                    }
                    bool operator==(const Iterator& o) const { return it_ == o.it_; }
                    bool operator!=(const Iterator& o) const { return !operator==(o); }
                    value_type* operator->() const { return &operator*(); }
                    value_type& operator*() const {
                        return vertex_->adjacency_lists_->vertices_[it_->target()];
                    }
            };

        template<Graph_type T_graph_type, typename V, typename E, typename D>
            template<bool T_is_const>
            class Adj_lists_vertex_base<T_graph_type, V, E, D>::Edges_iterator : public Iterator<T_is_const> {
                private:
                    using Base = Iterator<T_is_const>;
                    using link_type = typename Base::link_type;
                public:
                    using entry_type = Edges_iterator_entry<D, link_type, T_is_const>;
                private:
                    using vertex_base_type = typename Base::value_type;
                    using links_type = typename Base::links_type;
                    using links_iterator_type = typename Base::links_iterator_type;

                    entry_type entry_;

                    static auto links_end(Forward_list<link_type>& links) { return links.end(); }
                    static auto links_end(const Forward_list<link_type>& links) { return links.cend(); }

                    template<typename VV, typename EE, bool TT_is_const, typename It>
                        static void update_edge_p(Edges_iterator_entry<VV, EE, TT_is_const>& entry, const It& it) {
                            entry.edge_ = &*it;
                        }

                    void update_entry() {
                        if (Base::it_ != links_end(Base::vertex_->links_)) {
                            // todo target_ is redundant, delete
                            entry_.target_ = &Base::vertex_->adjacency_lists_->vertices_[Base::it_->target()];
                            update_edge_p(entry_, Base::it_);
                        }
                    }
                public:
                    Edges_iterator(vertex_base_type* vertex, const links_iterator_type& it)
                        :Base(vertex, it), entry_(vertex)
                    { update_entry(); }
                    const entry_type& operator*() const { return entry_; }
                    const entry_type* operator->() const { return &entry_; }
                    Edges_iterator& operator++() { 
                        Base::operator++();
                        update_entry();
                        return *this;
                    }
            };

    }

    template<Graph_type T_graph_type, typename V, typename TE = bool, typename E = Adjacency_lists_ns::Edge<TE>>
        using Adjacency_lists = Adjacency_lists_ns::Adjacency_lists<T_graph_type,
              Adjacency_lists_ns::Vertex<T_graph_type, V, E>, E>;

    // todo remove
    // template<Graph_type T_graph_type, typename V, typename TE = bool, typename E = Adjacency_lists_ns::Edge<TE>>
    //     using Adjacency_lists = Adjacency_lists_ns::Adjacency_lists<T_graph_type, V, E>;

    // template<typename V, typename C>
    //     using Network_flow = Adjacency_lists_ns::Adjacency_lists<Graph_type::FLOW, V, Adjacency_lists_ns::Flow_edge<C>>;

}
