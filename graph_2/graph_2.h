#pragma once

#include <map>

namespace Graph_2 {

template <typename F, typename S = F>
struct Pair {
    F m_first;
    S m_second;
    template <typename FF, typename SS>
    Pair(FF&& first, SS&& second)
        : m_first(std::forward<FF>(first)),
          m_second(std::forward<SS>(second)) {}
};

template <typename F, typename S = F>
struct Tie {
    F& m_first;
    S& m_second;
    Tie(F& begin, S& end) : m_first(begin), m_second(end) {}
    Tie& operator=(const Pair<F, S>& its) {
        m_first = its.m_first;
        m_second = its.m_second;
        return *this;
    }
};

template <typename G, typename T = typename G::value_type,
          typename V = typename G::vertex_type>
class Constructor {
   private:
    G& m_g;
    std::map<T, V> m_map;

   public:
    Constructor(G& g) : m_g(g) {}
    template <typename TT = typename G::value_type>
    V get_or_create(TT&& t) {
        auto it = m_map.find(std::forward<TT>(t));
        if (it == m_map.end()) {
            V v = m_g.add_vertex(std::forward<TT>(t));
            m_map.insert({std::forward<TT>(t), v});
            return v;
        }
        return it->second;
    }
    template <typename TT = typename G::value_type>
    Constructor& add_edge(TT&& t1, TT&& t2) {
        m_g.add_edge(get_or_create(std::forward<TT>(t1)),
                     get_or_create(std::forward<TT>(t2)));
        return *this;
    }
};

}  // namespace Graph_2
