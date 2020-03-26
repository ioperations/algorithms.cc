#include "gtest/gtest.h"

#include "adjacency_matrix.h"
#include "adjacency_lists.h"

using namespace Graph;

template<typename G>
void weighted_graphs_test(bool digraph = false) {
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
    if (digraph) {
        ASSERT_EQ(nullptr, g.get_edge(v2, v1));
        ASSERT_EQ(nullptr, g.get_edge(v3, v1));
    } else {
        ASSERT_EQ(4, g.get_edge(v2, v1)->weight());
        ASSERT_EQ(8, g.get_edge(v3, v1)->weight());
    }

    {
        const auto& gr = g;
        auto& v1 = gr[0];
        auto& v2 = gr[1];
        auto& v3 = gr[2];

        ASSERT_EQ(4, gr.get_edge(v1, v2)->weight());
        ASSERT_EQ(8, gr.get_edge(v1, v3)->weight());

        ASSERT_EQ(nullptr, gr.get_edge(v2, v3));
    }

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

    g.remove_edge(v1, v2);
    ASSERT_EQ(nullptr, g.get_edge(v1, v2));

    if (digraph) {
        g.add_edge(v1, v2, 4);
        g.add_edge(v2, v1, 4);
        g.remove_edge(v1, v2);
        ASSERT_EQ(nullptr, g.get_edge(v1, v2));
        ASSERT_EQ(4, g.get_edge(v2, v1)->weight());
    } else {
        g.add_edge(v1, v2, 4);
        ASSERT_EQ(4, g.get_edge(v1, v2)->weight());
        ASSERT_EQ(4, g.get_edge(v2, v1)->weight());
        g.remove_edge(v1, v2);
        ASSERT_EQ(nullptr, g.get_edge(v1, v2));
        ASSERT_EQ(nullptr, g.get_edge(v2, v1));
    }
}

template<typename G>
void bool_edges_test(bool digraph = false) {
    G g;
    auto& v1 = g.create_vertex(1);
    auto& v2 = g.create_vertex(2);
    g.add_edge(v1, v2);

    std::stringstream ss;
    for (auto e = v1.cedges_begin(); e != v1.cedges_end(); ++e)
        ss << e->source() << " " << e->target();
    ASSERT_EQ("1 2", ss.str());

    g.remove_edge(v1, v2);
    ASSERT_FALSE(g.has_edge(v1, v2));

    if (digraph) {
        g.add_edge(v1, v2);
        g.add_edge(v2, v1);
        g.remove_edge(v1, v2);
        ASSERT_FALSE(g.has_edge(v1, v2));
        ASSERT_TRUE(g.has_edge(v2, v1));
    } else {
        g.add_edge(v1, v2);
        ASSERT_TRUE(g.has_edge(v1, v2));
        ASSERT_TRUE(g.has_edge(v2, v1));
        g.remove_edge(v1, v2);
        ASSERT_FALSE(g.has_edge(v1, v2));
        ASSERT_FALSE(g.has_edge(v2, v1));
    }
}

TEST(Graph_test, base) {
    weighted_graphs_test<Adjacency_lists<Graph_type::GRAPH, int, int>>();
    weighted_graphs_test<Adjacency_lists<Graph_type::DIGRAPH, int, int>>(true);
    weighted_graphs_test<Adjacency_matrix<Graph_type::GRAPH, int, int>>();
    weighted_graphs_test<Adjacency_matrix<Graph_type::DIGRAPH, int, int>>(true);

    bool_edges_test<Adjacency_lists<Graph_type::GRAPH, int>>();
    bool_edges_test<Adjacency_lists<Graph_type::DIGRAPH, int>>(true);
    bool_edges_test<Adjacency_matrix<Graph_type::GRAPH, int>>();
    bool_edges_test<Adjacency_matrix<Graph_type::DIGRAPH, int>>(true);
}

