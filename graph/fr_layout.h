#pragma once

#include <string>
#include <vector>
#include <functional>
#include <utility>

#include <boost/graph/adjacency_list.hpp>

struct Vertex_position {
    std::string label_;
    double x_;
    double y_;
    Vertex_position(const std::string& label, double x, double y)
        :label_(label), x_(x), y_(y)
    {}
};

class Graph;
class Vertex_positions_impl;
class Vertex_positions {
    private:
        friend class Graph;
        Vertex_positions_impl* positions_impl_;
        Vertex_positions(Graph&);
    public:
        ~Vertex_positions();
        void iterate(std::function<void(const Vertex_position&)>);
        void iterate_edges(std::function<void(const std::pair<Vertex_position, Vertex_position>&)>);
};

class Vertex;
class Graph : public boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, boost::property<boost::vertex_name_t, std::string>> {
    private:
        Vertex_positions positions_;
    public:
        Graph();
        vertex_descriptor add_vertex(const std::string& label);
        Graph& add_edge(const vertex_descriptor& v1, const vertex_descriptor& v2);
        void calculate_layout(double width, double height, int iterations = 100);
        Vertex_positions& positions();
};

