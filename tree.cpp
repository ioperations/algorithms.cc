#include <cstddef>
#include <iostream>

#include "array.h"
#include "tree.h"
#include "tree_printer.h"

int main() {
    using node = Node<int>;
    using nodes = Array<node>;
    node n(1, nodes::build_array(
            node(11, nodes::build_array(111, 112, 113)),
            node(12, nodes::build_array(121, 122)),
            node(13, nodes::build_array(
                    node(131, nodes::build_array(1311, 1312, 1313)),
                    132, 133))
            ));
    Tree_printer().print(n, std::cout);

    std::cout << n;
}

