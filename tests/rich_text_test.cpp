#include "gtest/gtest.h"

#include <sstream>

#include "array.h"
#include "std_ext.h"
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

TEST(Rich_text, test_2) {
    using namespace Rich_text;
    Array<Entry<int>> array(4);

    Sequence s(array.begin(), array.end());
    std::stringstream ss;
    ss << std::endl;
    s.print_with_styled_entry(ss, Style::bold(), *array.begin(), *(array.begin() + 1));
    ss << std::endl;

    s.print_with_styled_entries(ss, 
          styled_entries(Style::bold(), *array.begin(), *(array.begin() + 1)),
          styled_entries(Style::red_bg(), *(array.begin() + 1), *(array.begin() + 2))
          );
    ss << std::endl;

    ASSERT_EQ(ss.str(), R"(
[1m0[0m [1m0[0m 0 0
[1m0[0m [1m[41m0[0m [41m0[0m 0
)");
}

