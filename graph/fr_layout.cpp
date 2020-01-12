#include "fr_layout.h"

#include <boost/graph/fruchterman_reingold.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/topology.hpp>

#include <iostream>
#include <vector>

namespace b = boost;

class Vertex_positions_impl {
    private:
        using Positions = std::vector<boost::rectangle_topology<>::point_type>;
        using Positions_map = boost::iterator_property_map<Positions::iterator, 
              boost::property_map<Graph, boost::vertex_index_t>::type>;
        Graph& graph_;
        Positions positions_;

        Vertex_position compose_position(const Graph::vertex_descriptor& v) {
            return {b::get(b::vertex_name, graph_, v), map_[v][0], map_[v][1]};
        }
    public:
        Positions_map map_;
        Vertex_positions_impl(Graph& g) :graph_(g) {}

        void update() {
            positions_ = Positions(num_vertices(graph_));
            map_ = Positions_map(positions_.begin(), get(b::vertex_index, graph_)); 
        }

        template<typename F>
            void iterate(F f) {
                b::graph_traits<Graph>::vertex_iterator vi, vi_end;
                for (b::tie(vi, vi_end) = b::vertices(graph_); vi != vi_end; ++vi) {
                    f(compose_position(*vi));
                }
            }

        template<typename F>
            void iterate_edges(F f) {
                b::graph_traits<Graph>::edge_iterator vi, vi_end;
                for (b::tie(vi, vi_end) = b::edges(graph_); vi != vi_end; ++vi) {
                    auto s = b::source(*vi, graph_);
                    auto t = b::target(*vi, graph_);
                    f({compose_position(s), compose_position(t)});
                }
            }
};


Vertex_positions::Vertex_positions(Graph& g) :positions_impl_(new Vertex_positions_impl(g)) {}

Vertex_positions::~Vertex_positions() { delete positions_impl_; }

void Vertex_positions::iterate(std::function<void(const Vertex_position&)> f) {
    positions_impl_->iterate(f);
}

void Vertex_positions::iterate_edges(std::function<void(const std::pair<Vertex_position, Vertex_position>&)> f) {
    positions_impl_->iterate_edges(f);
}

Graph::vertex_descriptor Graph::add_vertex(const std::string& label) {
    return b::add_vertex(label, *this);
}

Graph& Graph::add_edge(const vertex_descriptor& v1, const vertex_descriptor& v2) {
    b::add_edge(v1, v2, *this);
    return *this;
}

Graph::Graph() :positions_(*this) {}

void Graph::calculate_layout(double width, double height, int iterations) {
    positions_.positions_impl_->update();
    auto& positions = positions_.positions_impl_->map_;
    b::minstd_rand generator;
    b::rectangle_topology<> topology(generator, 0, 0, width, height);
    b::random_graph_layout(*this, positions, topology);
    b::fruchterman_reingold_force_directed_layout(*this, positions, topology);
}

Vertex_positions& Graph::positions() { return positions_; }
