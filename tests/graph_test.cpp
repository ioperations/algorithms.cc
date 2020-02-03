#include "gtest/gtest.h"

#include "adjacency_matrix.h"
#include "adjacency_lists.h"

using namespace Graph;

template<typename G>
void base_test() {
    G g;
    auto& v1 = g.create_vertex(1);
    auto& v2 = g.create_vertex(2);
    auto& v3 = g.create_vertex(3);
    g.add_edge(v1, v2, 4);
    g.add_edge(v1, v3, 8);

    ASSERT_EQ(1, v1.value());
    ASSERT_EQ(2, v2.value());
    ASSERT_EQ(3, v3.value());

    ASSERT_EQ(4, g.get_edge(v1, v2)->weight());
    ASSERT_EQ(8, g.get_edge(v1, v3)->weight());

    ASSERT_EQ(nullptr, g.get_edge(v2, v3));

    g.get_edge(v1, v2)->set_weight(10);
    g.get_edge(v1, v3)->set_weight(15);

    ASSERT_EQ(10, g.get_edge(v1, v2)->weight());
    ASSERT_EQ(15, g.get_edge(v1, v3)->weight());

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

template<typename G>
void bool_edges_test() {
    G g;
    auto& v1 = g.create_vertex(1);
    auto& v2 = g.create_vertex(2);
    g.add_edge(v1, v2);

    std::stringstream ss;
    for (auto e = v1.cedges_begin(); e != v1.cedges_end(); ++e)
        ss << e->source() << " " << e->target();
    ASSERT_EQ("1 2", ss.str());
}

TEST(Graph_test, base) {
    base_test<Adjacency_matrix<Graph_type::DIGRAPH, int, Adjacency_matrix_ns::Edge<int>>>();
    base_test<Adjacency_lists<Graph_type::DIGRAPH, int, Adjacency_lists_ns::Edge<int>>>();

    bool_edges_test<Adjacency_matrix<Graph_type::GRAPH, int>>();
    bool_edges_test<Adjacency_lists<Graph_type::GRAPH, int>>();
}

