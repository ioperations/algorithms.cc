#include "gtest/gtest.h"

#include "map.h"

TEST(Binary_tree, binary_tree) {
    Map<int, int> map;
    map.insert(1, 1);

    Map_printer<int, int> printer;
    printer.print(map, std::cout);
}
