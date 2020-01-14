#include "fr_layout.h"

#include "graph.h"
#include "adjacency_matrix.h"
#include "drawables.h"

class Drawables_stream {
    private:
        Vertical_drawable_block* const block_;
    public:
        Drawables_stream(Vertical_drawable_block* block) :block_(block) {}
        Vertical_drawable_block* const block() { return block_; }
};

Drawables_stream& operator<<(Drawables_stream& stream, const char* text) {
    stream.block()->add(new Drawable_text(text));
    return stream;
}

template<typename G>
Drawables_stream& operator<<(Drawables_stream& stream, const G& graph) {
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
    stream.block()->add(new Drawable_graph(calculator.calculate_layout_2(100, 100)));
    return stream;
}

Drawable* const compose_drawables() {
    Vertical_drawable_block* drawable = new Vertical_drawable_block;
    Drawables_stream dout(drawable);

    dout << "graph with Euler tour";
    Graph::Adjacency_matrix<int> graph;
    Graph::Constructor constructor(graph);
    constructor
        .add_edge(0, 1)
        .add_edge(0, 2)
        .add_edge(0, 5)
        .add_edge(0, 6)
        .add_edge(1, 2)
        .add_edge(2, 3)
        .add_edge(2, 4)
        .add_edge(3, 4)
        .add_edge(4, 5)
        .add_edge(4, 6);
    dout << graph;

    dout << "\ngraph with Hamilton path";
    constructor
        .add_edge(1, 3)
        .add_edge(3, 5);
    dout << graph;

    return drawable;
}

IMPLEMENT_CANVAS_APP(compose_drawables)

