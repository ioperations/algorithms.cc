#include "network_flow_min_cost.h"

#include "graphs.h"
#include "gtest/gtest.h"
#include "network_flow.h"

TEST(Parent_link_array_tree, max_flow_min_cost) {
    auto f = Graph::Samples::simplex_sample();
    Graph::Network_flow_ns::MaxFlowMinCost simplex(f, f[0], f[5], 200);
    std::stringstream ss;
    ss << std::endl;
    print_representation(f, ss);
    ASSERT_EQ(R"(
0: ->1(2/3[3]) ->2(2/3[1]) 
1: <-0(2/3[3]) ->3(2/2[1]) ->4(0/2[1]) 
2: <-0(2/3[1]) ->3(0/1[4]) ->4(2/2[2]) 
3: <-1(2/2[1]) <-2(0/1[4]) ->5(2/2[2]) 
4: <-1(0/2[1]) <-2(2/2[2]) ->5(2/2[1]) 
5: <-3(2/2[2]) <-4(2/2[1]) 
)",
              ss.str());
    ASSERT_EQ(20, calculate_network_flow_cost(f));
}

