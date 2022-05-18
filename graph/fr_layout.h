#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "forward_list.h"

namespace Graph {

namespace Layout {

struct VertexPosition {
    std::string m_label;
    double m_x;
    double m_y;
    VertexPosition() = default;
    VertexPosition(const std::string& label, double x, double y)
        : m_label(label), m_x(x), m_y(y) {}
};

class Calculator;
class Vertex_positions_impl;
class VertexPositions {
   private:
    friend class Calculator;
    Vertex_positions_impl* m_positions_impl;
    VertexPositions(Calculator&);

   public:
    ~VertexPositions();
    void iterate(std::function<void(const VertexPosition&)>);
    void iterate_edges(
        std::function<void(const std::pair<VertexPosition, VertexPosition>&)>);
};

class Layout {
   public:
    using Edges =
        ForwardList<std::pair<VertexPosition* const, VertexPosition* const>>;

   private:
    std::vector<VertexPosition> m_positions;
    Edges m_edges;
    size_t m_edges_count;

   public:
    Layout(std::vector<VertexPosition>&& positions, Edges&& edges,
           size_t edges_count)
        : m_positions(std::move(positions)),
          m_edges(std::move(edges)),
          m_edges_count(edges_count) {}
    Layout(const Layout&) = delete;
    Layout& operator=(const Layout&) = delete;
    Layout(Layout&& o)
        : m_positions(std::move(o.m_positions)),
          m_edges(std::move(o.m_edges)),
          m_edges_count(o.m_edges_count) {}
    Layout& operator=(Layout&&) = delete;

    auto edges_cbegin() const { return m_edges.cbegin(); }
    auto edges_cend() const { return m_edges.cend(); }
    auto vertices_cbegin() const { return m_positions.cbegin(); }
    auto vertices_cend() const { return m_positions.cend(); }
    auto edges_count() { return m_edges_count; }
    auto vertices_count() { return m_positions.size(); }
};

class Vertex;
class Calculator {
   private:
    friend class Vertex_positions_impl;
    using graph_type = boost::adjacency_list<
        boost::listS, boost::vecS, boost::undirectedS,
        boost::property<boost::vertex_name_t, std::string>>;
    graph_type m_graph;
    VertexPositions m_positions;

   public:
    using vertex_descriptor = typename graph_type::vertex_descriptor;
    Calculator();
    vertex_descriptor add_vertex(const std::string& label);
    Calculator& add_edge(const vertex_descriptor& v1,
                         const vertex_descriptor& v2);
    void calculate_layout(double width, double height, int iterations = 100);
    VertexPositions& positions();

    Layout calculate_layout_2(double size, int iterations = 100);
};

}  // namespace Layout

}  // namespace Graph
