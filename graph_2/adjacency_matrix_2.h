#pragma once

#include <vector>

#include "array.h"
#include "graph_2.h"
#include "two_dimensional_array.h"

namespace Graph_2 {

namespace Adjency_matrix_ns {

class Vertex {
   private:
    size_t index_;

   public:
    Vertex(size_t index) : index_(index) {}
    Vertex() : Vertex(0) {}
    operator size_t() { return index_; }
};

class Adjacent_iterator_end {
   private:
    template <typename T>
    friend class Adjency_matrix;
    static const Adjacent_iterator_end instance() {
        static const Adjacent_iterator_end e;
        return e;
    }
};

class Adjacent_iterator {
   private:
    template <typename T>
    friend class Adjency_matrix;
    bool* begin_;
    bool* end_;
    bool* current_;
    Adjacent_iterator(bool* begin, bool* end, bool* current)
        : begin_(begin), end_(end), current_(current) {}
    Adjacent_iterator& move_on_first() {
        while (!*current_ && current_ != end_) ++current_;
        return *this;
    }

   public:
    Adjacent_iterator() = default;
    Adjacent_iterator& operator++() {
        ++current_;
        return move_on_first();
    }
    bool operator!=(const Adjacent_iterator& o) {
        return current_ != o.current_;
    }
    bool operator!=(const Adjacent_iterator_end& e) { return current_ != end_; }
    Vertex operator*() { return current_ - begin_; }
};

template <typename T>
class Adjency_matrix {
   private:
    Array<T> vertices_;
    Two_dimensional_array<bool> edges_;
    size_t vertices_count_;

   public:
    using value_type = T;
    using vertex_type = Vertex;
    using adjacent_iterator = Adjacent_iterator;
    using adjacent_iterator_end = Adjacent_iterator_end;

    Adjency_matrix(size_t size = 100)
        : vertices_(size), edges_(size, size), vertices_count_(0) {
        edges_.fill(false);
    }
    template <typename TT>
    vertex_type add_vertex(TT&& t) {
        auto index = vertices_count_;
        vertices_[index] = std::forward<TT>(t);
        ++vertices_count_;
        return index;
    }
    T& value(vertex_type v) { return vertices_[v]; }
    void add_edge(vertex_type v, vertex_type w) {
        edges_.get(v, w) = true;
        edges_.get(w, v) = true;
    }
    Pair<adjacent_iterator, adjacent_iterator_end> adjacent(vertex_type v) {
        auto r = edges_[v];
        bool* b = r.begin();
        bool* e = r.end();
        return {Adjacent_iterator(b, e, b).move_on_first(),
                Adjacent_iterator_end::instance()};
    }

    void print_internal(std::ostream& stream) {
        for (size_t r = 0; r < vertices_count_; ++r) {
            for (size_t c = 0; c < vertices_count_; ++c)
                stream << edges_.get(r, c) << " ";
            stream << std::endl;
        }
    }
};
}  // namespace Adjency_matrix_ns

template <typename T>
using Adjency_matrix = typename Adjency_matrix_ns::Adjency_matrix<T>;

}  // namespace Graph_2
