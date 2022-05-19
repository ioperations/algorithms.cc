#pragma once

#include "math.h"

class Fraction {
   private:
    long m_numerator;
    long m_denominator;
    Fraction& normalize_and_get() {
        auto gcd = greatest_common_divisor(m_numerator, m_denominator);
        if (gcd > 1) {
            m_numerator /= gcd;
            m_denominator /= gcd;
        }
        return *this;
    }
    inline static long numerator_to_lcm(const Fraction& f, long lcm) {
        return lcm * f.m_numerator / f.m_denominator;
    }
    inline long sum_numerators(const Fraction& o, long lcm, int sign = 1) {
        return numerator_to_lcm(*this, lcm) + sign * numerator_to_lcm(o, lcm);
    }
    inline Fraction add_or_substract(const Fraction& o, int sign = 1) {
        auto lcm = least_common_multiple(m_denominator, o.m_denominator);
        return Fraction(sum_numerators(o, lcm, sign), lcm).normalize_and_get();
    }
    inline Fraction& add_or_substract_assign(const Fraction& o, int sign = 1) {
        auto lcm = least_common_multiple(m_denominator, o.m_denominator);
        m_numerator = sum_numerators(o, lcm, sign);
        m_denominator = lcm;
        return this->normalize_and_get();
    }

   public:
    Fraction(long numerator, long denominator = 1)
        : m_numerator(numerator), m_denominator(denominator) {}
    Fraction operator+(const Fraction& o) { return add_or_substract(o); }
    Fraction& operator+=(Fraction& o) { return add_or_substract_assign(o); }

    Fraction operator-(const Fraction& o) { return add_or_substract(o, -1); }
    Fraction& operator-=(Fraction& o) { return add_or_substract_assign(o, -1); }

    Fraction operator*(const Fraction& o) const {
        return Fraction(m_numerator * o.m_numerator,
                        m_denominator * o.m_denominator)
            .normalize_and_get();
    }
    Fraction& operator*=(const Fraction& o) {
        m_numerator *= o.m_numerator;
        m_denominator *= o.m_denominator;
        return this->normalize_and_get();
    }

    Fraction operator/(const Fraction& o) const {
        return Fraction(m_numerator * o.m_denominator,
                        m_denominator * o.m_numerator)
            .normalize_and_get();
    }
    Fraction& operator/=(const Fraction& o) {
        m_numerator *= o.m_denominator;
        m_denominator *= o.m_numerator;
        return this->normalize_and_get();
    }

    Fraction& operator/=(int n) {
        m_denominator *= n;
        return this->normalize_and_get();
    }
    Fraction operator/(int n) const {
        return Fraction(m_numerator, m_denominator * n).normalize_and_get();
    }

    bool operator==(const Fraction& o) const {
        return m_numerator == o.m_numerator && m_denominator == o.m_denominator;
    }
    double to_double() const { return (double)m_numerator / m_denominator; }
    long numerator() const { return m_numerator; }
    long denominator() const { return m_denominator; }
};

inline std::ostream& operator<<(std::ostream& stream, const Fraction f) {
    stream << f.numerator();
    if (f.denominator() > 1) stream << "/" << f.denominator();
    return stream;
}
