#include <iostream>

#include "graph_2.h"
#include "adjacency_matrix_2.h"

int main() {
    using Graph = typename Graph_2::Adjency_matrix<int>;
    Graph g;

    Graph_2::Constructor(g)
        .add_edge(1, 2)
        .add_edge(1, 3)
        .add_edge(2, 3);

    g.print_internal(std::cout);

    Graph::adjacent_iterator v;
    Graph::adjacent_iterator_end e;
    for (Graph_2::Tie(v, e) = g.adjacent(0); v != e; ++v)
        std::cout << *v << std::endl;
    for (Graph_2::Tie(v, e) = g.adjacent(1); v != e; ++v)
        std::cout << *v << std::endl;

}
