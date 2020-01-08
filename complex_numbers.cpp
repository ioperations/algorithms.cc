#include <cmath>
#include <iostream>

template<typename T>
struct Complex {
    T re_;
    T im_;
    Complex(T re, T im) :re_(re), im_(im) {}
    Complex operator*(const Complex<T>& o) const {
        return Complex(re_ * o.re_ - im_ * o.im_,
                       re_ * o.im_ + im_ * o.re_);
    }
    Complex& operator*=(const Complex<T>& o) {
        re_ = re_ * o.re_ - im_ * o.im_;
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
    int n = 8;
    for (int i = 0; i < n; ++i) {
        float theta = 2.0 * M_PI * i / n;
        Complex<double> c(cos(theta), sin(theta)), x = c;

        for (int j = 0; j < n - 1; ++j) 
            x *= c;
        printf("%d: %7.3f %7.3fi %7.3f %7.3fi\n", i, c.re_, c.im_, x.re_, x.im_);
    }
}
