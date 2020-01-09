#include <string>

#include "adjacency_matrix.h"

int main() {
    Adjacency_matrix<std::string> graph;
    auto v1 = graph.create_vectex("v1");
    auto v2 = graph.create_vectex("v2");
    graph.add_edge(v1, v2);

    std::cout << v1 << std::endl;
    std::cout << v2 << std::endl;
}
