#pragma once

#include <map>

namespace Graph_2 {

template <typename F, typename S = F>
struct Pair {
    F first_;
    S second_;
    template <typename FF, typename SS>
    Pair(FF&& first, SS&& second)
        : first_(std::forward<FF>(first)), second_(std::forward<SS>(second)) {}
};

template <typename F, typename S = F>
struct Tie {
    F& first_;
    S& second_;
    Tie(F& begin, S& end) : first_(begin), second_(end) {}
    Tie& operator=(const Pair<F, S>& its) {
        first_ = its.first_;
        second_ = its.second_;
        return *this;
    }
};

template <typename G, typename T = typename G::value_type,
          typename V = typename G::vertex_type>
class Constructor {
   private:
    G& g_;
    std::map<T, V> map_;

   public:
    Constructor(G& g) : g_(g) {}
    template <typename TT = typename G::value_type>
    V get_or_create(TT&& t) {
        auto it = map_.find(std::forward<TT>(t));
        if (it == map_.end()) {
            V v = g_.add_vertex(std::forward<TT>(t));
            map_.insert({std::forward<TT>(t), v});
            return v;
        } else
            return it->second;
    }
    template <typename TT = typename G::value_type>
    Constructor& add_edge(TT&& t1, TT&& t2) {
        g_.add_edge(get_or_create(std::forward<TT>(t1)),
                    get_or_create(std::forward<TT>(t2)));
        return *this;
    }
};

}  // namespace Graph_2
