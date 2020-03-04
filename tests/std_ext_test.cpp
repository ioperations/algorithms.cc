#include "gtest/gtest.h"

#include <sstream>

#include "std_ext.h"

TEST(Std_ext_test, std_array) {
    auto a = Std_ext::make_array(1, 2, 3);
    std::stringstream ss;
    ss << a[0] << " " << a[1] << " " << a[2];
    ASSERT_EQ(ss.str(), "1 2 3");

    ss = std::stringstream();
    Std_ext::for_each(a, [&ss](auto& i) { ss << i << " "; });
    ASSERT_EQ(ss.str(), "1 2 3 ");
}
