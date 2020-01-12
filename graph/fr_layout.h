#pragma once

#include <string>
#include <vector>
#include <functional>
#include <utility>

#include <boost/graph/adjacency_list.hpp>

namespace Graph {

    namespace Layout {

        struct Vertex_position {
            std::string label_;
            double x_;
            double y_;
            Vertex_position(const std::string& label, double x, double y)
                :label_(label), x_(x), y_(y)
            {}
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
                void iterate_edges(std::function<void(const std::pair<Vertex_position, Vertex_position>&)>);
        };

        class Vertex;
        class Calculator {
            private:
                friend class Vertex_positions_impl;
                using graph_type = boost::adjacency_list<
                    boost::listS, boost::vecS, boost::undirectedS, boost::property<boost::vertex_name_t, std::string>>;
                graph_type graph_;
                Vertex_positions positions_;
            public:
                using vertex_descriptor = typename graph_type::vertex_descriptor;
                Calculator();
                vertex_descriptor add_vertex(const std::string& label);
                Calculator& add_edge(const vertex_descriptor& v1, const vertex_descriptor& v2);
                void calculate_layout(double width, double height, int iterations = 100);
                Vertex_positions& positions();
        };

    }

}
