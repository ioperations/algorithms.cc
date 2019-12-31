#include "gtest/gtest.h"

#include "map.h"

TEST(Binary_tree, binary_tree) {
    Map<int, int> map;
    map.insert(10, 1);
    map.insert(20, 2);
    map.insert(50, 3);
    map.insert(40, 4);
    map.insert(5, 5);
    map.insert(21, 6);
    map.insert(21, 7);

    Map_printer<int, int> printer;
    printer.print(map, std::cout);

    std::cout << std::endl;

    map.iterate();
}
