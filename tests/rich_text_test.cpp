#include "gtest/gtest.h"

#include <sstream>

#include "array.h"
#include "std_ext.h"
#include "rich_text.h"

namespace Rich_text {

    template<typename T, std::size_t SZ>
        struct Styled_entries {
            Style style_;
            std::array<T, SZ> entries_;
            Styled_entries(Style style, std::array<T, SZ>&& entries) :style_(style), entries_(entries) {
                Std_ext::for_each(entries_, [this](auto e) {
                    e->add_style(style_); });
            }
            ~Styled_entries() {
                Std_ext::for_each(entries_, [this](auto e) {
                    e->remove_style(style_); });
            }
        };

    template<typename T, typename... Args, std::size_t SZ = sizeof...(Args) + 1>
        auto styled_entries(const Style&& style, T&& t, Args&&... args) {
            std::array<T, SZ> entries;
            Std_ext::fill_array(entries, std::forward<T>(t), std::forward<Args>(args)...);
            return Styled_entries<T, SZ>(std::move(style), std::move(entries));
        }

    template<typename It, typename... SES>
        void print(std::ostream& stream, const It& begin, const It& end, SES&&... styled_entries) {
            for (auto el = begin; el != end; ++el)
                stream << *el << " ";
        }
}

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

    Array<Rich_text::Entry<int>> array;

    print(std::cout, array.begin(), array.end(),
          styled_entries(Style::bold(), &*array.begin()),
          styled_entries(Style::red_bg(), &*(array.begin() + 1))
          );
    std::cout << std::endl;
}
