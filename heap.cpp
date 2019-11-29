#include "heap.h"

#include <iostream>


int main() {
    using node = Binary_tree_node<int>;
    node root(1,
              new node(2, new node(21), new node(22)),
              new node(3, new node(31)));

    auto heap = tree_to_heap(root);
    std::cout << "heap:" << std::endl;
    print_heap(heap);
}
