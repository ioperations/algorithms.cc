#pragma once

#include "math.h"

class Fraction {
    private:
        long numerator_;
        long denominator_;
        Fraction& normalize_and_get() {
            auto gcd = greatest_common_divisor(numerator_, denominator_);
            if (gcd > 1) {
                numerator_ /= gcd;
                denominator_ /= gcd;
            }
            return *this;
        }
        inline static long numerator_to_lcm(const Fraction& f, long lcm) {
            return lcm * f.numerator_ / f.denominator_;
        }
        inline long sum_numerators(const Fraction& o, long lcm, int sign = 1) {
            return numerator_to_lcm(*this, lcm) + sign * numerator_to_lcm(o, lcm);
        }
        inline Fraction add_or_substract(const Fraction& o, int sign = 1) {
            auto lcm = least_common_multiple(denominator_, o.denominator_);
            return Fraction(sum_numerators(o, lcm, sign), lcm).normalize_and_get();
        }
        inline Fraction& add_or_substract_assign(const Fraction& o, int sign = 1) {
            auto lcm = least_common_multiple(denominator_, o.denominator_);
            numerator_ = sum_numerators(o, lcm, sign);
            denominator_ = lcm;
            return this->normalize_and_get();
        }
    public:
        Fraction(long numerator, long denominator = 1) 
            :numerator_(numerator), denominator_(denominator) 
        {}
        Fraction operator+(const Fraction& o) {
            return add_or_substract(o);
        }
        Fraction& operator+=(Fraction& o) {
            return add_or_substract_assign(o);
        }

        Fraction operator-(const Fraction& o) {
            return add_or_substract(o, -1);
        }
        Fraction& operator-=(Fraction& o) {
            return add_or_substract_assign(o, -1);
        }

        Fraction operator*(const Fraction& o) const {
            return Fraction(numerator_ * o.numerator_, denominator_ * o.denominator_)
                .normalize_and_get();
        }
        Fraction& operator*=(const Fraction& o) {
            numerator_ *= o.numerator_;
            denominator_*= o.denominator_;
            return this->normalize_and_get();
        }

        Fraction operator/(const Fraction& o) const {
            return Fraction(numerator_ * o.denominator_, denominator_ * o.numerator_)
                .normalize_and_get();
        }
        Fraction& operator/=(const Fraction& o) {
            numerator_ *= o.denominator_;
            denominator_*= o.numerator_;
            return this->normalize_and_get();
        }

        Fraction& operator/=(int n) {
            denominator_*= n;
            return this->normalize_and_get();
        }
        Fraction operator/(int n) const {
            return Fraction(numerator_, denominator_ * n).normalize_and_get();
        }

        bool operator==(const Fraction& o) const {
            return numerator_ == o.numerator_ && denominator_ == o.denominator_;
        }
        double to_double() const {
            return (double) numerator_ / denominator_;
        }
        long numerator() const { return numerator_; }
        long denominator() const { return denominator_; }
};

std::ostream& operator<<(std::ostream& stream, const Fraction f) {
    stream << f.numerator();
    if (f.denominator() > 1)
        stream << "/" << f.denominator();
    return stream;
}

