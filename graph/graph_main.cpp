#include <string>

#include "adjacency_matrix.h"
#include "adjacency_lists.h"

template<typename G>
void test_graph(G& graph) {
    auto& v1 = graph.create_vertex("v1");
    auto& v2 = graph.create_vertex("v2");
    graph.add_edge(v1, v2);
    std::cout << v1 << std::endl;
    std::cout << v2 << std::endl;
    std::cout << "internal structure: " << std::endl;
    graph.print_internal(std::cout);
}

int main() {
    std::cout << std::endl;
    {
        std::cout << "adjacency matrix:" << std::endl;
        Adjacency_matrix<std::string> graph;
        test_graph(graph);
    }
    {
        std::cout << "adjacency lists:" << std::endl;
        Adjacency_lists<std::string> graph;
        test_graph(graph);
    }
}
