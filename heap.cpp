#include "heap.h"

#include <iostream>
#include <vector>

int main() {
    std::cout << "---===---" << std::endl;

    Heap<int> h;
    h.push(1);
    h.push(5);
    h.push(2);
    h.push(9);
    h.push(3);
    h.push(11);

    while (!h.empty())
        std::cout << h.pop() << " ";
    std::cout << std::endl;

    h = {};
    h.push(1);
    h.push(2);
    while (!h.empty())
        std::cout << h.pop() << " ";
    std::cout << std::endl;

    using node = Binary_tree_node<int>;
    node root(1,
              new node(2, new node(21), new node(22)),
              new node(3, new node(31), new node(32)));
    h = {root};

    std::cout << h << std::endl;
    while (!h.empty())
        std::cout << h.pop() << " ";
    std::cout << std::endl;
}
