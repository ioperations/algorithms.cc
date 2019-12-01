#include "heap.h"

#include <iostream>

int main() {
    using node = Binary_tree_node<int>;
    node root(1,
              new node(2, new node(21), new node(22)),
              new node(3, new node(31), new node(32)));

    std::cout << root << std::endl;

    Heap<int> heap(root);
    std::cout << heap << std::endl;

    // std::cout << heap[6] << std::endl;
    // heap.fix_down(0);
    // heap.fix_up(6);
    heap.fix_down(0);
    std::cout << heap << std::endl;
    // heap[0] = 5;
    // std::cout << heap << std::endl;
}
