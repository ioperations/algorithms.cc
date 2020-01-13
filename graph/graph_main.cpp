#include "adjacency_matrix.h"
#include "adjacency_lists.h"

#include <string>
#include <stdexcept>
#include <sstream>

template<typename It>
void print_path(const It& b, const It& e) {
    Graph::print_collection(b, e, " - ", [](auto p) { return *p; }, std::cout);
}

template<typename G>
void test_graph(G& graph) {
    Graph::Constructor constructor(graph);

    constructor.add_edge("1", "2")
        .add_edge("2", "3")
        .add_edge("2", "4")
        .add_edge("3", "4");
    constructor.get_or_create_vertex("5");

    auto has_simple_path = [&graph, &constructor](const std::string& l1, const std::string& l2) {
        std::cout << l1 << " - " << l2 << ": ";
        if (Graph::has_simple_path(graph, constructor.get_vertex(l1), constructor.get_vertex(l2)))
            std::cout << "simple path found";
        else
            std::cout << "no simple path";
        std::cout << std::endl;
    };
    has_simple_path("1", "4");
    has_simple_path("1", "5");

    std::cout << "Hamilton path: ";
    auto h_path = Graph::compose_hamilton_path(graph);
    print_path(h_path.begin(), h_path.end());

    std::cout << std::endl << "internal structure: " << std::endl;
    graph.print_internal(std::cout);
}

int main() {
    std::cout << std::endl;
    {
        std::cout << "adjacency matrix:" << std::endl;
        Graph::Adjacency_matrix<std::string> graph;
        test_graph(graph);
    }
    {
        std::cout << "adjacency lists:" << std::endl;
        Graph::Adjacency_lists<std::string> graph;
        test_graph(graph);
    }
    {
        std::cout << "graph with Euler tour" << std::endl;
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

        auto path = Graph::compose_euler_tour(graph, graph.vertex_at(0));
        print_path(path.begin(), path.end());
        std::cout << std::endl;

        std::cout << "graph with hamilton path" << std::endl;
        constructor
            .add_edge(1, 3)
            .add_edge(3, 5);
        auto h_path = Graph::compose_hamilton_path(graph);
        print_path(h_path.begin(), h_path.end());
        std::cout << std::endl;

        Graph::dfs(graph,
                   [](const auto& v) {
                       std::cout << v << std::endl;
                   },
                   [](const auto& v, const auto& w) {
                       if (v.index() < w.index())
                           std::cout << v << " - " << w << std::endl;
                   });
    }
}
