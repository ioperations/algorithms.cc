#pragma once

#include "array.h"

namespace Graph {

template <typename T>
class Counters : public Array<T> {
   private:
    using Base = Array<T>;
    T m_current_max;

   public:
    using value_type = T;
    static constexpr T default_value() { return static_cast<T>(-1); }
    Counters(size_t size) : Base(size), m_current_max(default_value()) {
        Base::fill(default_value());
    }
    bool is_unset(size_t index) {
        return Base::operator[](index) == default_value();
    }
    void set_next(size_t index) { Base::operator[](index) = ++m_current_max; }
    Array<T> to_array() {
        Array<T> array(std::move(*this));
        return array;
    }
};

template <>
class Counters<bool> : public Array<bool> {
   private:
    using Base = Array<bool>;

   public:
    using value_type = bool;
    static constexpr bool default_value() { return false; }
    Counters(size_t size) : Base(size) { Base::fill(default_value()); }
    bool is_unset(size_t index) {
        return Base::operator[](index) == default_value();
    }
    void set_next(size_t index) { Base::operator[](index) = true; }
};

template <typename G, typename T_pre, typename D>
class DfsBase {
   protected:
    using vertex_type = typename G::vertex_type;
    using edge_type = typename G::vertex_type::const_edges_iterator::entry_type;
    const G& m_g;
    Counters<T_pre> m_pre;
    D* m_d;

   public:
    DfsBase(const G& g)
        : m_g(g), m_pre(g.vertices_count()), m_d(static_cast<D*>(this)) {}
    void search() {
        for (auto v = m_g.cbegin(); v != m_g.cend(); ++v)
            if (m_pre.is_unset(*v)) m_d->search_vertex(*v);
    }
    void search_vertex(const vertex_type& v) {
        m_d->visit_vertex(v);
        m_pre.set_next(v);
        for (auto e = v.cedges_begin(); e != v.cedges_end(); ++e) {
            if (m_pre.is_unset(e->target())) search_vertex(e->target());
            m_d->visit_edge(*e);
        }
        m_d->search_post_process(v);
    }
};

template <typename G, typename T_pre, typename D>
class Dfs : public DfsBase<G, T_pre, D> {
   private:
    using Base = DfsBase<G, T_pre, D>;

   public:
    using vertex_type = typename Base::vertex_type;
    Dfs(const G& g) : Base(g) {}
    void visit_vertex(const vertex_type& v) {}
    void search_post_process(const vertex_type& v) {}
};

template <typename G, typename T_pre, typename T_post, typename D>
class PostDfsBase : public DfsBase<G, T_pre, D> {
   private:
    using Base = DfsBase<G, T_pre, D>;

   protected:
    using vertex_type = typename Base::vertex_type;
    using edge_type = typename Base::edge_type;

   public:
    Counters<T_post> m_post;
    PostDfsBase(const G& g) : Base(g), m_post(g.vertices_count()) {}
    void visit_vertex(const vertex_type& v) {}
    void visit_edge(const edge_type& e) {}
    void search_post_process(const vertex_type& v) { m_post.set_next(v); }
};

template <typename G, typename T_pre, typename T_post>
class PostDfs
    : public PostDfsBase<G, T_pre, T_post, PostDfs<G, T_pre, T_post>> {
   private:
    using Base = PostDfsBase<G, T_pre, T_post, PostDfs<G, T_pre, T_post>>;

   public:
    PostDfs(const G& g) : Base(g) {}
};

template <typename G, typename V = typename G::vertex_type,
          typename T_v_visitor, typename T_e_visitor>
void dfs(const G& g, T_v_visitor v_visitor, T_e_visitor e_visitor) {
    class Searcher : public Dfs<G, bool, Searcher> {
       private:
        using Base = Dfs<G, bool, Searcher>;
        using vertex_type = typename Base::vertex_type;
        using edge_type = typename Base::edge_type;
        T_v_visitor m_v_visitor;
        T_e_visitor m_e_visitor;

       public:
        Searcher(const G& g, T_v_visitor v_visitor, T_e_visitor e_visitor)
            : Base(g), m_v_visitor(v_visitor), m_e_visitor(e_visitor) {}
        void visit_vertex(const vertex_type& v) { m_v_visitor(v); }
        void visit_edge(const edge_type& e) { m_e_visitor(e); }
    };
    Searcher(g, v_visitor, e_visitor).search();
}

}  // namespace Graph
