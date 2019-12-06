#include <iostream>
#include <climits>
#include <bitset>

#include "array.h"
#include "random.h"
#include "stack.h"

template<typename It>
void print_seq(const It& begin, const It& end) {
    for (auto el = begin; el != end; ++el)
        std::cout << *el << " ";
    std::cout << std::endl;
}

template<typename T>
inline unsigned char get_bit(T t, unsigned int digit) { 
    return (t & (T(1) << digit)) > 0 ? 1 : 0;
}

template<typename It>
void radix_quick_sort(const It& begin, const It& end) {
    using value_type = typename std::iterator_traits<It>::value_type;

    auto print_bits = [begin, end]() {
        for (auto el = begin; el != end; ++el)
            std::cout << std::bitset<CHAR_BIT * sizeof(value_type)>(*el) << std::endl;
    };

    struct Interval {
        It begin_;
        It end_;
        unsigned int digit_;
        Interval(It begin, It end, unsigned int digit) :begin_(begin), end_(end), digit_(digit) {}
    };
    Stack<Interval> stack;

    stack.emplace(begin, end, sizeof(value_type) * CHAR_BIT - 1);

    print_seq(begin, end);
    print_bits();
    while (!stack.empty()) {
        auto interval = stack.pop();
        if (interval.begin_ < interval.end_ && interval.digit_ > 0) {
            auto i = interval.begin_;
            auto j = interval.end_ - 1;
            while (i != j) {
                while (get_bit(*i, interval.digit_) == 0 && i < j) ++i;
                while (get_bit(*j, interval.digit_) == 1 && j > i) --j;
                std::swap(*i, *j);
            }
            if (get_bit(*(interval.end_ - 1), interval.digit_) == 0) ++j;
            stack.emplace(j, interval.end_, interval.digit_ - 1);
            stack.emplace(interval.begin_, j, interval.digit_ - 1);
        }
    }
    print_seq(begin, end);
    print_bits();
}

int main() {
    using type = unsigned char;
    std::cout << std::endl;
    Random_sequence_generator<type> generator(400, 'A', 'Z');
    auto array = generator.generate_array<Array<type>>(15);
    radix_quick_sort(array.begin(), array.end());
}
