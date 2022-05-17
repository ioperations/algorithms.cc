#include "fraction.h"

#include "gtest/gtest.h"

TEST(Fraction_test, test_1) {
    Fraction f1(3, 8);
    Fraction f2(5, 12);

    ASSERT_EQ(Fraction(19, 24), f1 + f2);
    auto f1_copy = f1;
    ASSERT_EQ(Fraction(19, 24), f1_copy += f2);

    ASSERT_EQ(Fraction(-1, 24), f1 - f2);
    f1_copy = f1;
    ASSERT_EQ(Fraction(-1, 24), f1_copy -= f2);

    ASSERT_EQ(Fraction(5, 32), f1 * f2);
    f1_copy = f1;
    ASSERT_EQ(Fraction(5, 32), f1_copy *= f2);

    ASSERT_EQ(Fraction(9, 10), f1 / f2);
    f1_copy = f1;
    ASSERT_EQ(Fraction(9, 10), f1_copy /= f2);
}
