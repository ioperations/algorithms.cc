#include "map.h"

#include "array.h"
#include "gtest/gtest.h"

TEST(Binary_tree, binary_tree) {
    Array<int> a{10, 20, 50, 40, 5, 21, 21};
    Map<int, int> map;
    int i = 0;
    for (auto& key : a) {
        map.insert(key, ++i);
    }

    Map_printer<int, int> printer;
    printer.print(map, std::cout);
    std::cout << std::endl;

    map = {};
    i = 0;
    for (auto& key : a) {
        map.insert_root(key, ++i);
    }
    printer.print(map, std::cout);
    std::cout << std::endl;

    map.iterate();
}
