#include "fr_layout.h"

#include "graph.h"
#include "graphs.h"
#include "adjacency_matrix.h"
#include "adjacency_lists.h"
#include "drawables.h"

class Drawables_stream {
    private:
        Vertical_drawable_block* const block_;
    public:
        Drawables_stream(Vertical_drawable_block* block) :block_(block) {}
        Vertical_drawable_block* const block() { return block_; }
};

Drawables_stream& operator<<(Drawables_stream& stream, const std::string& text) {
    stream.block()->add(new Drawable_text(text));
    return stream;
}

template<typename G>
Drawables_stream& print_graph(Drawables_stream& stream, const G& graph) {
    Graph::Layout::Calculator calculator;
    std::map<size_t, Graph::Layout::Calculator::vertex_descriptor> map;
    Graph::dfs(graph,
               [&calculator, &map](const auto& v) {
                   map[v.index()] = calculator.add_vertex(std::to_string(v.value()));
               },
               [&calculator, &map](const auto& v, const auto& w) {
                   if (v.index() < w.index())
                       calculator.add_edge(map.find(v.index())->second, map.find(w.index())->second);
               });
    stream.block()->add(new Drawable_graph(calculator.calculate_layout_2(200, 1000)));
    return stream;
}

template<typename T>
Drawables_stream& operator<<(Drawables_stream& stream, 
                             const Graph::Adjacency_matrix<Graph::Graph_type::GRAPH, T>& graph) {
    return print_graph(stream, graph);
}

template<typename T>
Drawables_stream& operator<<(Drawables_stream& stream,
                             const Graph::Adjacency_lists<T>& graph) {
    return print_graph(stream, graph);
}

template<typename It>
void print_path(Drawables_stream& dout, const It& b, const It& e) {
    std::stringstream ss;
    Graph::print_collection(b, e, " - ", [](auto p) { return *p; }, ss);
    dout << ss.str();
}

Drawable* const compose_drawables() {
    Vertical_drawable_block* drawable = new Vertical_drawable_block;
    Drawables_stream dout(drawable);

    using graph_type = Graph::Adjacency_lists<int>;

    dout << "graph with Euler tour";
    auto graph = Graph::Samples::euler_tour_sample<graph_type>();
    dout << graph;

    auto path = Graph::compose_euler_tour(graph, graph.vertex_at(0));
    print_path(dout, path.cbegin(), path.cend());

    dout << "\ngraph with Hamilton path";

    graph = Graph::Samples::hamilton_path_sample<graph_type>();
    dout << graph;
    auto h_path = Graph::compose_hamilton_path(graph);
    print_path(dout, h_path.cbegin(), h_path.cend());

    dout <<"\ngraph with bridges";
    graph = Graph::Samples::bridges_sample<graph_type>();
    dout << graph;

    std::stringstream ss;
    auto bridges = Graph::find_bridges(graph);
    auto b = bridges.begin();
    if (b != bridges.end()) {
        auto print_bridge = [&ss](auto& b) {
            ss << *b->first << " - " << *b->second;
        };
        print_bridge(b);
        for (++b; b != bridges.end(); ++b) {
            ss << ", ";
            print_bridge(b);
        }
    }
    dout << ss.str();

    graph = Graph::Samples::shortest_paths_sample<graph_type>();
    dout << graph;

    return drawable;
}

IMPLEMENT_CANVAS_APP(compose_drawables)

