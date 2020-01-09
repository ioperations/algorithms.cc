#include "adjacency_matrix.h"
#include "adjacency_lists.h"

#include <string>
#include <map>

template<typename G>
void test_graph(G& graph) {
    std::map<std::string, typename G::Vertex&> map;
    auto add_edge = [&map, &graph](const std::string& l1, const std::string& l2) {
        auto get_vertex = [&map, &graph](const std::string& l) -> typename G::Vertex& {
            auto it = map.find(l);
            if (it == map.end()) {
                auto& v = graph.create_vertex(l);
                map.insert({l, v});
                return v;
            } else
                return it->second;
        };
        auto& v1 = get_vertex(l1);
        auto& v2 = get_vertex(l2);
        graph.add_edge(v1, v2);
    };

    add_edge("1", "2");
    add_edge("2", "3");
    add_edge("2", "4");
    add_edge("3", "4");

    std::cout << "internal structure: " << std::endl;
    graph.print_internal(std::cout);
}

int main() {
    std::cout << std::endl;
    {
        std::cout << "adjacency matrix:" << std::endl;
        Adjacency_matrix<std::string> graph;
        test_graph(graph);
        graph.iterate();
    }
    {
        std::cout << "adjacency lists:" << std::endl;
        Adjacency_lists<std::string> graph;
        test_graph(graph);
    }
}
