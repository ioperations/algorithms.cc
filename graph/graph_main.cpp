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
}
