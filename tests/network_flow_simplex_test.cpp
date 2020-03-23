#include "gtest/gtest.h"

#include "network_flow.h"
#include "network_flow_simplex.h"
#include "graph.h"
#include "graphs.h"
#include "forward_list.h"

using namespace Graph::Network_flow_ns;

using graph_type = Graph::Network_flow_with_cost<int, int>;
using vertex_type = typename graph_type::vertex_type;
using link_type = typename graph_type::link_type;
using tree_type = Parent_link_array_tree<link_type>;

template<typename T>
std::string stringify(const T& t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
}

void add_link(graph_type& g, tree_type& tree, vertex_type& v, vertex_type& w) { 
    tree[w] = g.add_edge(v, w, 5, 0, 0);
}

void add_to_tree(tree_type& tree, graph_type& graph, Forward_list<std::pair<size_t, size_t>>&& links_map) {
    // TODO remove 
    for (auto e : links_map)
        tree[e.second] = graph.get_edge(e.first, e.second)->link();
}

struct Replace_test_case {
    graph_type g_;
    tree_type tree_;
    Versions_array<unsigned int> versions_;
    Replace_test_case(graph_type&& g, Forward_list<std::pair<size_t, size_t>>&& links_map)
        :g_(std::move(g)), tree_(g_.vertices_count()), versions_(g_.vertices_count()) {
            add_to_tree(tree_, g_, std::move(links_map));
        }
};


TEST(Parent_link_array_tree, find_lca) {
    graph_type g;
    auto& v0 = g.create_vertex(0);
    auto& v1 = g.create_vertex(1);
    auto& v2 = g.create_vertex(2);
    auto& v3 = g.create_vertex(3);
    auto& v4 = g.create_vertex(4);
    auto& v5 = g.create_vertex(5);
    auto& v6 = g.create_vertex(6);

    tree_type tree(g.vertices_count());
    add_link(g, tree, v6, v0);
    add_link(g, tree, v0, v1);
    add_link(g, tree, v0, v2);
    add_link(g, tree, v1, v3);
    add_link(g, tree, v2, v4);
    add_link(g, tree, v4, v5);

    ASSERT_EQ("6-0, 0-1, 0-2, 1-3, 2-4, 4-5, --6", stringify(tree));

    g.add_edge(v3, v6, 5, 0, 0);
    g.add_edge(v5, v6, 5, 0, 0);

    Versions_array<unsigned int> versions_array(g.vertices_count());

    ASSERT_EQ(0, tree.find_lca(&v3, &v5, versions_array)->index());
    ASSERT_TRUE(tree.is_between(&v5, &v0, &v2));
}

TEST(Parent_link_array_tree, find_lca_2) {
    auto g = Graph::Samples::simplex_sample();
    g.add_edge(g[5], g[0], 200, 150, 100);
    Replace_test_case t(std::move(g), {{5, 0}, {3, 1}, {0, 2}, {2, 3}, {1, 4}});
    ASSERT_EQ(2, t.tree_.find_lca(&t.g_[2], &t.g_[4], t.versions_)->index());
}

TEST(Parent_link_array_tree, replace) {
    graph_type g;
    auto& v0 = g.create_vertex(0);
    auto& v1 = g.create_vertex(1);
    auto& v2 = g.create_vertex(2);
    auto& v3 = g.create_vertex(3);
    auto& v4 = g.create_vertex(4);
    auto& v5 = g.create_vertex(5);
    auto& v6 = g.create_vertex(6);
    auto& v7 = g.create_vertex(7);

    tree_type tree(g.vertices_count());
    add_link(g, tree, v7, v0);
    add_link(g, tree, v0, v1);
    add_link(g, tree, v0, v2);
    add_link(g, tree, v1, v3);
    add_link(g, tree, v2, v5);
    add_link(g, tree, v2, v6);
    add_link(g, tree, v5, v4);

    g.add_edge(v1, v4, 5, 0, 0);
    g.add_edge(v3, v7, 5, 0, 0);
    g.add_edge(v4, v7, 5, 0, 0);
    g.add_edge(v6, v7, 5, 0, 0);

    ASSERT_EQ("7-0, 0-1, 0-2, 1-3, 5-4, 2-5, 2-6, --7", stringify(tree));

    auto old_link = tree[2];
    ASSERT_EQ(0, old_link->source());
    ASSERT_EQ(2, old_link->target());
    auto new_link = g.get_edge(v1, v4)->link();
    ASSERT_EQ(1, new_link->source());
    ASSERT_EQ(4, new_link->target());

    Versions_array<unsigned int> versions_array(g.vertices_count());
    tree.replace(old_link, new_link, versions_array);

    std::cout << tree << std::endl;
    ASSERT_EQ("7-0, 0-1, 5-2, 1-3, 1-4, 4-5, 2-6, --7", stringify(tree));
}

template<typename T>
void do_print(const T& t) {
    std::cout << t << std::endl;
}

TEST(Parent_link_array_tree, replace_2) {
    auto g = Graph::Samples::simplex_sample();

    tree_type tree(g.vertices_count());
    tree[0] = g.add_edge(g[5], g[0], 200, 150, 100);
    auto add_to_tree = [&g, &tree](size_t v, size_t w) {
        tree[w] = g.get_edge(v, w)->link();
    };
    add_to_tree(5, 0);
    add_to_tree(0, 1);
    add_to_tree(3, 2);
    add_to_tree(5, 3);
    add_to_tree(2, 4);

    Versions_array<unsigned int> versions_array(g.vertices_count());
    // tree.replace(g.get_edge(3, 5)->link(), g.get_edge(4, 5)->link(), versions_array);
    // ASSERT_EQ("5-0, 0-1, 4-2, 2-3, 5-4, --5", stringify(tree));

    // ++versions_array;
    // std::stringstream ss;
    // for (size_t i = 0; i < 6; ++i)
    //     ss << tree.get_vertex_potential(g[i], versions_array) << " ";
    // ASSERT_EQ("-100 -103 3 -1 1 0 ", ss.str());

    // do_print(*g.get_edge(0, 1)->link());

    ++versions_array;
    tree.augment(g.get_edge(4, 5)->link(), versions_array);

    // std::cout << tree << std::endl;
}

TEST(Parent_link_array_tree, replace_3) {
    auto g = Graph::Samples::simplex_sample();
    g.add_edge(g[5], g[0], 200, 150, 100); // TODO doesn't participate in the test, delete?
    Replace_test_case t(std::move(g), {{1, 0}, {4, 1}, {0, 2}, {2, 3}, {5, 4}});
    t.tree_.replace(t.g_.get_link(1, 4), t.g_.get_link(2, 4), t.versions_);
    ASSERT_EQ("2-0, 0-1, 4-2, 2-3, 5-4, --5", stringify(t.tree_));
}

TEST(Parent_link_array_tree, replace_4) {
    auto g = Graph::Samples::simplex_sample();
    g.add_edge(g[5], g[0], 200, 150, 100);
    Replace_test_case t(std::move(g), {{5, 0}, {3, 1}, {0, 2}, {5, 3}, {1, 4}});
    t.tree_.replace(t.g_.get_link(3, 5), t.g_.get_link(4, 5), t.versions_);
    ASSERT_EQ("5-0, 4-1, 0-2, 1-3, 5-4, --5", stringify(t.tree_));
}

TEST(Parent_link_array_tree, augment) {
    auto g = Graph::Samples::simplex_sample();
    tree_type tree(g.vertices_count());
    tree[0] = g.add_edge(g[5], g[0], 200, 150, 100);
    auto add_to_tree = [&g, &tree](size_t v, size_t w) {
        tree[w] = g.get_edge(v, w)->link();
    };
    add_to_tree(5, 0);
    add_to_tree(0, 1);
    add_to_tree(0, 2);
    add_to_tree(1, 3);
    add_to_tree(1, 4);
    std::cout << tree << std::endl;

    Versions_array<unsigned int> versions_array(g.vertices_count());
    tree.augment(g.get_edge(2, 3)->link(), versions_array);
}
