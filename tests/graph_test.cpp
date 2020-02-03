#include "gtest/gtest.h"

#include "adjacency_matrix.h"
#include "adjacency_lists.h"

using namespace Graph;

template<typename G>
void base_test() {
    G gw;
    auto& v1 = gw.create_vertex(1);
    auto& v2 = gw.create_vertex(2);
    auto& v3 = gw.create_vertex(3);
    gw.add_edge(v1, v2, 4);
    gw.add_edge(v1, v3, 8);

    ASSERT_EQ(1, v1.value());
    ASSERT_EQ(2, v2.value());
    ASSERT_EQ(3, v3.value());

    ASSERT_EQ(4, gw.get_edge(v1, v2)->weight());
    ASSERT_EQ(8, gw.get_edge(v1, v3)->weight());

    ASSERT_EQ(nullptr, gw.get_edge(v2, v3));

    gw.get_edge(v1, v2)->set_weight(10);
    gw.get_edge(v1, v3)->set_weight(15);

    ASSERT_EQ(10, gw.get_edge(v1, v2)->weight());
    ASSERT_EQ(15, gw.get_edge(v1, v3)->weight());

    v1.edges_begin()->source().set_value(45);
    v1.edges_begin()->edge().set_weight(20);

    std::string expected(R"(
45 2 20
45 3 15
)");
    std::stringstream ss;
    ss << std::endl;
    for (auto e = v1.edges_begin(); e != v1.edges_end(); ++e)
        ss << e->source() << " " << e->target() << " " << e->edge().weight() << std::endl;
    ASSERT_EQ(expected, ss.str());
    ss = std::stringstream();
    ss << std::endl;
    for (auto e = v1.cedges_begin(); e != v1.cedges_end(); ++e)
        ss << e->source() << " " << e->target() << " " << e->edge().weight() << std::endl;
    ASSERT_EQ(expected, ss.str());
}

TEST(Graph_test, base) {
    base_test<Adjacency_matrix<Graph_type::DIGRAPH, int, Adjacency_matrix_ns::Edge<int>>>();
    base_test<Adjacency_lists<Graph_type::DIGRAPH, int, Adjacency_lists_ns::Edge<int>>>();
}

