#include "fr_layout.h"

#include <boost/graph/fruchterman_reingold.hpp>
#include <boost/graph/random_layout.hpp>
#include <boost/graph/topology.hpp>

#include <iostream>
#include <vector>

namespace Graph {

    namespace Layout {

        namespace b = boost;

        class Vertex_positions_impl {
            private:
                using Positions = std::vector<boost::rectangle_topology<>::point_type>;
                using Positions_map = boost::iterator_property_map<Positions::iterator, 
                      boost::property_map<Calculator::graph_type, boost::vertex_index_t>::type>;
                Calculator& calculator_;
                Positions positions_;

                Vertex_position compose_position(const Calculator::vertex_descriptor& v) {
                    return {b::get(b::vertex_name, calculator_.graph_, v), map_[v][0], map_[v][1]};
                }
            public:
                Positions_map map_;
                Vertex_positions_impl(Calculator& c) :calculator_(c) {}

                void update() {
                    positions_ = Positions(num_vertices(calculator_.graph_));
                    map_ = Positions_map(positions_.begin(), get(b::vertex_index, calculator_.graph_)); 
                }

                template<typename F>
                    void iterate(F f) {
                        b::graph_traits<Calculator::graph_type>::vertex_iterator vi, vi_end;
                        for (b::tie(vi, vi_end) = b::vertices(calculator_.graph_); vi != vi_end; ++vi) {
                            f(compose_position(*vi));
                        }
                    }

                template<typename F>
                    void iterate_edges(F f) {
                        b::graph_traits<Calculator::graph_type>::edge_iterator vi, vi_end;
                        for (b::tie(vi, vi_end) = b::edges(calculator_.graph_); vi != vi_end; ++vi) {
                            auto s = b::source(*vi, calculator_.graph_);
                            auto t = b::target(*vi, calculator_.graph_);
                            f({compose_position(s), compose_position(t)});
                        }
                    }
        };


        Vertex_positions::Vertex_positions(Calculator& g) :positions_impl_(new Vertex_positions_impl(g)) {}

        Vertex_positions::~Vertex_positions() { delete positions_impl_; }

        void Vertex_positions::iterate(std::function<void(const Vertex_position&)> f) {
            positions_impl_->iterate(f);
        }

        void Vertex_positions::iterate_edges(std::function<void(const std::pair<Vertex_position, Vertex_position>&)> f) {
            positions_impl_->iterate_edges(f);
        }

        Calculator::vertex_descriptor Calculator::add_vertex(const std::string& label) {
            return b::add_vertex(label, graph_);
        }

        Calculator& Calculator::add_edge(const vertex_descriptor& v1, const vertex_descriptor& v2) {
            b::add_edge(v1, v2, graph_);
            return *this;
        }

        Calculator::Calculator() :positions_(*this) {}

        void Calculator::calculate_layout(double width, double height, int iterations) {
            positions_.positions_impl_->update();
            auto& positions = positions_.positions_impl_->map_;
            b::minstd_rand generator;
            b::rectangle_topology<> topology(generator, 0, 0, width, height);
            b::random_graph_layout(graph_, positions, topology);
            b::fruchterman_reingold_force_directed_layout(graph_, positions, topology);
        }

        Vertex_positions& Calculator::positions() { return positions_; }

    }

}
