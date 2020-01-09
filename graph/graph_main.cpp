#include <string>

#include "adjacency_matrix.h"
#include "adjacency_lists.h"

int main() {
    {
        Adjacency_matrix<std::string> graph;
        auto& v1 = graph.create_vertex("v1");
        auto& v2 = graph.create_vertex("v2");
        graph.add_edge(v1, v2);

        std::cout << v1 << std::endl;
        std::cout << v2 << std::endl;
    }

    {
        Adjacency_lists<std::string> graph;
        auto& v1 = graph.create_vertex("v1");
        auto& v2 = graph.create_vertex("v2");
        graph.add_edge(v1, v2);
        std::cout << v1 << std::endl;
        std::cout << v2 << std::endl;
    }
}
