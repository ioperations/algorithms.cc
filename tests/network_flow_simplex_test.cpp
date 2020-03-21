#include "gtest/gtest.h"

#include "network_flow.h"
#include "network_flow_simplex.h"

using namespace Graph::Network_flow_ns;

TEST(Network_flow, find_lca) {
    using graph_type = Graph::Network_flow<int, int>;
    graph_type f;
    auto& v0 = f.create_vertex(0);
    auto& v1 = f.create_vertex(1);
    auto& v2 = f.create_vertex(2);
    auto& v3 = f.create_vertex(3);
    auto& v4 = f.create_vertex(4);
    auto& v5 = f.create_vertex(5);
    auto& v6 = f.create_vertex(6);

    Parent_link_array_tree<typename graph_type::link_type> tree(f.vertices_count());
    auto add_link = [&f, &tree](auto& v, auto& w) { tree[w] = f.add_edge(v, w, 5, 0); };
    add_link(v6, v0);
    add_link(v0, v1);
    add_link(v0, v2);
    add_link(v1, v3);
    add_link(v2, v4);
    add_link(v4, v5);

    std::stringstream ss;
    ss << tree;
    ASSERT_EQ("6-0, 0-1, 0-2, 1-3, 2-4, 4-5, --6", ss.str());

    f.add_edge(v3, v6, 5, 0);
    f.add_edge(v5, v6, 5, 0);

    Versions_array<unsigned int> versions_array(f.vertices_count());

    ASSERT_EQ(0, tree.find_lca(&v3, &v5, &v6, versions_array)->index());
    ASSERT_TRUE(tree.is_between(&v5, &v0, &v2));
}

TEST(Network_flow, replace) {
    using graph_type = Graph::Network_flow<int, int>;
    graph_type f;
    auto& v0 = f.create_vertex(0);
    auto& v1 = f.create_vertex(1);
    auto& v2 = f.create_vertex(2);
    auto& v3 = f.create_vertex(3);
    auto& v4 = f.create_vertex(4);
    auto& v5 = f.create_vertex(5);
    auto& v6 = f.create_vertex(6);
    auto& v7 = f.create_vertex(7);

    Parent_link_array_tree<typename graph_type::link_type> tree(f.vertices_count());
    auto add_link = [&f, &tree](auto& v, auto& w) { tree[w] = f.add_edge(v, w, 5, 0); };
    add_link(v7, v0);
    add_link(v0, v1);
    add_link(v0, v2);
    add_link(v1, v3);
    add_link(v2, v5);
    add_link(v2, v6);
    add_link(v5, v4);

    f.add_edge(v1, v4, 5, 0);
    f.add_edge(v3, v7, 5, 0);
    f.add_edge(v4, v7, 5, 0);
    f.add_edge(v6, v7, 5, 0);

    std::stringstream ss;
    ss << tree;
    ASSERT_EQ("7-0, 0-1, 0-2, 1-3, 5-4, 2-5, 2-6, --7", ss.str());

    auto old_link = tree[2];
    ASSERT_EQ(0, old_link->source());
    ASSERT_EQ(2, old_link->target());
    auto new_link = f.get_edge(v1, v4)->link();
    ASSERT_EQ(1, new_link->source());
    ASSERT_EQ(4, new_link->target());

    Versions_array<unsigned int> versions_array(f.vertices_count());
    tree.replace(old_link, new_link, &v7, versions_array);

    ss = std::stringstream();
    ss << tree;
    ASSERT_EQ("7-0, 0-1, 5-2, 1-3, 1-4, 4-5, 2-6, --7", ss.str());
}

