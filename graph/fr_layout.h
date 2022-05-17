#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "forward_list.h"

namespace Graph {

namespace Layout {

struct Vertex_position {
    std::string label_;
    double x_;
    double y_;
    Vertex_position() = default;
    Vertex_position(const std::string& label, double x, double y)
        : label_(label), x_(x), y_(y) {}
};

class Calculator;
class Vertex_positions_impl;
class Vertex_positions {
   private:
    friend class Calculator;
    Vertex_positions_impl* positions_impl_;
    Vertex_positions(Calculator&);

   public:
    ~Vertex_positions();
    void iterate(std::function<void(const Vertex_position&)>);
    void iterate_edges(
        std::function<
            void(const std::pair<Vertex_position, Vertex_position>&)>);
};

class Layout {
   public:
    using Edges =
        Forward_list<std::pair<Vertex_position* const, Vertex_position* const>>;

   private:
    std::vector<Vertex_position> positions_;
    Edges edges_;
    size_t edges_count_;

   public:
    Layout(std::vector<Vertex_position>&& positions, Edges&& edges,
           size_t edges_count)
        : positions_(std::move(positions)),
          edges_(std::move(edges)),
          edges_count_(edges_count) {}
    Layout(const Layout&) = delete;
    Layout& operator=(const Layout&) = delete;
    Layout(Layout&& o)
        : positions_(std::move(o.positions_)),
          edges_(std::move(o.edges_)),
          edges_count_(o.edges_count_) {}
    Layout& operator=(Layout&&) = delete;

    auto edges_cbegin() const { return edges_.cbegin(); }
    auto edges_cend() const { return edges_.cend(); }
    auto vertices_cbegin() const { return positions_.cbegin(); }
    auto vertices_cend() const { return positions_.cend(); }
    auto edges_count() { return edges_count_; }
    auto vertices_count() { return positions_.size(); }
};

class Vertex;
class Calculator {
   private:
    friend class Vertex_positions_impl;
    using graph_type = boost::adjacency_list<
        boost::listS, boost::vecS, boost::undirectedS,
        boost::property<boost::vertex_name_t, std::string>>;
    graph_type graph_;
    Vertex_positions positions_;

   public:
    using vertex_descriptor = typename graph_type::vertex_descriptor;
    Calculator();
    vertex_descriptor add_vertex(const std::string& label);
    Calculator& add_edge(const vertex_descriptor& v1,
                         const vertex_descriptor& v2);
    void calculate_layout(double width, double height, int iterations = 100);
    Vertex_positions& positions();

    Layout calculate_layout_2(double size, int iterations = 100);
};

}  // namespace Layout

}  // namespace Graph
