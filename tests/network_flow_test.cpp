#include "network_flow.h"

#include "graph_common.h"
#include "graphs.h"
#include "gtest/gtest.h"
#include "test_utils.h"

using namespace Graph;
using namespace Graph::Network_flow_ns;

TEST(Network_flow_test, max_flow) {
    std::stringstream ss;
    auto g = Samples::flow_sample();
    Max_flow m(g, g[0], g[5], g.vertices_count() * 10);
    print_representation(g, reset_with_new_line(ss));
    ASSERT_EQ(R"(
0: ->1(2/2) ->2(2/3) 
1: <-0(2/2) ->3(1/3) ->4(1/1) 
2: <-0(2/3) ->3(1/1) ->4(1/1) 
3: <-1(1/3) <-2(1/1) ->5(2/2) 
4: <-1(1/1) <-2(1/1) ->5(2/3) 
5: <-3(2/2) <-4(2/3) 
)",
              ss.str());
}

TEST(Network_flow_test, pre_flow_push_max_flow) {
    std::stringstream ss;
    auto g = Samples::flow_sample();
    Pre_flow_push_max_flow m(g, g[0], g[5], g.vertices_count() * 10);
    print_representation(g, reset_with_new_line(ss));
    ASSERT_EQ(R"(
0: ->1(2/2) ->2(2/3) 
1: <-0(2/2) ->3(1/3) ->4(1/1) 
2: <-0(2/3) ->3(1/1) ->4(1/1) 
3: <-1(1/3) <-2(1/1) ->5(2/2) 
4: <-1(1/1) <-2(1/1) ->5(2/3) 
5: <-3(2/2) <-4(2/3) 
)",
              ss.str());
}

TEST(Network_flow_test, feasible_flow) {
    auto g = Samples::flow_sample();
    auto p = find_feasible_flow(g, std::map<int, int>{{0, 3}, {1, 3}, {3, 1}},
                                std::map<int, int>{{2, 1}, {4, 1}, {5, 5}});
    std::stringstream ss;
    for (auto e = p.second->cedges_begin(); e != p.second->cedges_end(); ++e) {
        auto& link = *e->edge().link();
        ss << link.other(*p.second) << ": " << link.flow() << "/" << link.cap()
           << "; ";
    }
    ASSERT_EQ("2: 1/1; 4: 1/1; 5: 3/5; ", ss.str());
}

TEST(Network_flow_test, bipartite_matching) {
    std::map<int, Forward_list<int>> mapping = {
        {0, {6, 7, 8}}, {1, {6, 7, 11}},  {2, {8, 9, 10}},
        {3, {6, 7}},    {4, {9, 10, 11}}, {5, {8, 10, 11}}};
    std::stringstream ss;
    for (auto e : bipartite_matching(mapping))
        ss << e.first << ": " << e.second << ", ";
    ASSERT_EQ("0: 7, 1: 11, 2: 8, 3: 6, 4: 9, 5: 10, ", ss.str());
}
