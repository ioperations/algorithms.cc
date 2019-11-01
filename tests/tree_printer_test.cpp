#include "gtest/gtest.h"

#include "tree.h"
#include "tree_printer.h"

TEST(Tree_printer_test, test_0) {
    using node = Node<int>;
    using nodes = Array<node>;

    auto to_string = [](const node& node) {
        std::stringstream ss;
        ss << std::endl;
        ss << node;
        return ss.str();
    };

    node n(1);
    ASSERT_EQ(to_string(n), "\n1");

    n = {1, nodes::build_array(11)};
    ASSERT_EQ(to_string(n), R"(
 1
 │
11)");

    n = {1, nodes::build_array(1, 2)};
    ASSERT_EQ(to_string(n), R"(
  1
┌─┴┐
1  2)");

    n = {1, nodes::build_array(node(1, nodes::build_array(4)), 2, 3)};
    ASSERT_EQ(to_string(n), R"(
   1
┌──┼──┐
1  2  3
│
4)");

    n = {1, nodes::build_array(
            node(11, nodes::build_array(111, 112, 113)),
            node(12, nodes::build_array(121, 122)),
            node(13, nodes::build_array(
                    node(131, nodes::build_array(1311, 1312, 1313)),
                    132, 133))
            )};

    ASSERT_EQ(to_string(n), R"(
                   1
      ┌────────────┼───────────┐
     11           12          13
 ┌────┼────┐    ┌──┴─┐    ┌────┼────┐
111  112  113  121  122  131  132  133
                    ┌─────┼─────┐
                  1311  1312  1313)");

    n = {1, nodes::build_array(
            node(11, nodes::build_array(111, 112, 113)),
            node(12, nodes::build_array(
                    121, 
                    node(122, nodes::build_array(1221, 1222, 1223)))),
            node(13, nodes::build_array(
                    node(131, nodes::build_array(1311, 1312, 1313)),
                    node(132, nodes::build_array(1321, 1322)),
                    133))
            )};
    ASSERT_EQ(to_string(n), R"(
                            1
      ┌────────────┬────────┴────────────────────┐
     11           12                            13
 ┌────┼────┐    ┌──┴─┐                 ┌─────────┴────┬────┐
111  112  113  121  122               131            132  133
               ┌─────┼─────┐     ┌─────┼─────┐     ┌──┴──┐
             1221  1222  1223  1311  1312  1313  1321  1322)");
}

