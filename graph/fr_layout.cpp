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

        Layout Calculator::calculate_layout_2(double width, double height, int iterations) { // todo rename

            using Positions = std::vector<boost::rectangle_topology<>::point_type>;
            using Positions_map = boost::iterator_property_map<Positions::iterator, 
                  boost::property_map<Calculator::graph_type, boost::vertex_index_t>::type>;

            Positions positions(num_vertices(graph_));
            Positions_map positions_map(positions.begin(), get(b::vertex_index, graph_)); 

            b::minstd_rand generator;
            b::rectangle_topology<> topology(generator, 0, 0, width, height);
            b::random_graph_layout(graph_, positions_map, topology);
            b::fruchterman_reingold_force_directed_layout(graph_, positions_map, topology);

            class Composer {
                private:
                    std::vector<Vertex_position> positions_;
                    size_t index_;
                    std::map<vertex_descriptor, Vertex_position*> positions_map_;
                    typename Layout::Edges edges_;
                    size_t edges_count_;
                public:
                    Composer(size_t size) :positions_(size), index_(0), edges_count_(0) {}
                    auto add(const vertex_descriptor& v, Vertex_position&& p) {
                        positions_[index_] = std::move(p);
                        auto pos = &positions_[index_];
                        positions_map_[v] = pos;
                        ++index_;
                        return pos;
                    }
                    auto find(const vertex_descriptor& v) {
                        return positions_map_.find(v);
                    }
                    auto map_end() {
                        return positions_map_.end();
                    }
                    void add_edge(Vertex_position* const v1, Vertex_position* const v2) {
                        edges_.push_back(std::make_pair(v1, v2));
                        ++edges_count_;
                    }
                    Layout to_layout() {
                        return {std::move(positions_), std::move(edges_), edges_count_};
                    }
            };
            Composer composer(b::num_vertices(graph_));

            b::graph_traits<Calculator::graph_type>::edge_iterator vi, vi_end;
            for (b::tie(vi, vi_end) = b::edges(graph_); vi != vi_end; ++vi) {
                auto get_or_create_pos = [this, &composer, &positions_map](auto v) {
                    Vertex_position* position;
                    auto entry = composer.find(v);
                    if (entry == composer.map_end())
                        position = composer.add(
                            v, {b::get(b::vertex_name, graph_, v), positions_map[v][0], positions_map[v][1]});
                    else
                        position = entry->second;
                    return position;
                };

                auto s = get_or_create_pos(b::source(*vi, graph_));
                auto t = get_or_create_pos(b::target(*vi, graph_));
                composer.add_edge(s, t);
            }

            return composer.to_layout();
        }

    }


}
