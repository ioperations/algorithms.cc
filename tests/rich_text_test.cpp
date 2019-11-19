#include "gtest/gtest.h"

#include <sstream>

#include "rich_text.h"

TEST(Rich_text, test_1) {
    using namespace Rich_text;

    Entry<int> r(12);
    std::stringstream ss;
    ss << std::endl << r << std::endl;

    r.add_style(Style::bold());
    ss << r << std::endl;

    r.add_style(Style::red_bg());
    ss << r << std::endl;

    r.remove_style(Style::bold());
    ss << r << std::endl;

    r.remove_style(Style::red_bg());
    ss << r << std::endl;

    r.add_style(Style::bold());
    r.add_style(Style::bold());
    r.remove_style(Style::bold());
    r.remove_style(Style::bold());
    ss << r << std::endl;

    ASSERT_EQ(ss.str(), R"(
12
[1m12[0m
[1m[41m12[0m
[41m12[0m
12
12
)");
}

