#include <cmath>
#include <iostream>

#include "math.h"
#include "array.h"

template<typename T>
struct Complex {
    T re_;
    T im_;
    Complex(T re, T im) :re_(re), im_(im) {}
    Complex operator*(const Complex<T>& o) {
        return *this *= o;
    }
    Complex& operator*=(const Complex<T>& o) {
        re_ = re_ * o.re_ + im_ * o.im_;
        im_ = re_ * o.im_ + im_ * o.re_;
        return *this;
    }
};
template<typename T>
std::ostream& operator<<(std::ostream& stream, const Complex<T> c) {
    stream << c.re_ << " + " << c.im_ << "i";
    return stream;
}

int main() {

    std::cout << sin(M_PI / 4) << std::endl;
    std::cout << sin_tailor(M_PI / 4) << std::endl;

    // int n = 8;
    for (int i = 1; i < 2; ++i) {
        // float theta = 2.0 * M_PI * i / n;
        // sin_tailor(theta);
        // std::cout << sin(theta) << std::endl;
        // std::cout << cos(theta) << std::endl;
        // std::cout << sin(M_PI / 2 - theta) << std::endl;
        // Complex<double> c(cos(theta), sin(theta)), x = c;
        // std::cout << i << ": " << c << " ";
        // for (int j = 0; j < n - 1; ++j) {
        //     x *= c;
        // }
        // std::cout << x << std::endl;
    }
}
