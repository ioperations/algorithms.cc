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

template<typename It>
void index_sort(const It& begin, const It& end) {
    if (begin == end)
        return;
    auto el = begin;
    auto min = *el;
    auto max = *el;
    for (++el; el != end; ++el) {
        min = std::min(min, *el);
        max = std::max(max, *el);
    }
    std::cout << "min key: " << min << std::endl;
    std::cout << "max key: " << max << std::endl;

    Array<unsigned int> positions(max - min + 1);
    for (auto& position : positions)
        position = 0;
    for (auto el = begin; el != end; ++el)
        positions[*el - min + 1]++;
    std::cout << "counts: " << std::endl << positions << std::endl;
    for (size_t i = 1; i < positions.size(); ++i)
        positions[i] += positions[i - 1];
    std::cout << "positions: " << std::endl << positions << std::endl;
    Array<int> aux(end - begin);
    for (long int i = 0; i < end - begin; ++i)
        aux[positions[begin[i] - min]++] = begin[i];
    for (long int i = 0; i < end - begin; ++i)
        begin[i] = aux[i];
}

template<typename It>
void msd_radix_sort(const It& begin, const It& end) {
    if (begin == end)
        return;
    Array<unsigned int> positions(UCHAR_MAX + 1);
    Array<std::string> aux(end - begin);

    struct Frame {
        const It begin_;
        const It end_;
        const size_t char_index_;
        Frame(const It& begin, const It& end, const size_t char_index)
            :begin_(begin), end_(end), char_index_(char_index)
        {}
    };
    Stack<Frame> stack;
    stack.emplace(begin, end, 0);

    while (!stack.empty()) {
        auto f = stack.pop();

        for (auto& e : positions)
            e = 0;
        auto char_of = [char_index = f.char_index_](std::string& s) -> unsigned char {
            return char_index < s.size()
                ? s[char_index]
                : 0;
        };
        for (auto s = f.begin_; s != f.end_; ++s)
            ++positions[char_of(*s) + 1];
        for (size_t i = 1; i < positions.size(); ++i)
            positions[i] += positions[i - 1];

        size_t max_length = 0;
        for (long int i = 0; i < f.end_ - f.begin_; ++i) {
            std::string& str = f.begin_[i];
            max_length = std::max(max_length, str.size());
            auto& pos = positions[char_of(str)];
            aux[pos] = std::move(f.begin_[i]);
            ++pos;
        }
        for (long i = 0; i < f.end_ - f.begin_; ++i)
            f.begin_[i] = aux[i];

        if (f.char_index_ + 1 < max_length)
            for (long b = 0; b < f.end_ - f.begin_; ) {
                auto e = positions[f.begin_[b][f.char_index_]];
                if (e - b > 1)
                    stack.emplace(f.begin_ + b, f.begin_ + e, f.char_index_ + 1);
                b = e;
            }
    }
}

int main() {
    {
        auto array = Random_sequence_generator<int>(300, 10, 99).generate_array<Array<int>>(15);
        std::cout << array << std::endl;

        Array<int> ids(100);
        for (size_t i = 0; i < array.size(); ++i)
            ids[array[i]] = array[i];
        std::cout << ids << std::endl;
        index_sort(array.begin(), array.end());
        std::cout << array << std::endl;
    }

    {
        using type = unsigned char;
        Random_sequence_generator<type> generator(400, 'A', 'Z');
        auto array = generator.generate_array<Array<type>>(15);
        radix_quick_sort(array.begin(), array.end());
    }

    {
        Array<std::string> text{"to", "achieve", "good", "performance", "using", "radix", "sort", "for",
            "a", "particular", "application", "we", "need", "to", "limit", "the", "number", "of", "empty",
            "bins", "encountered", "by", "choosing", "appropriate", "values", "both", "for", "the", "radix",
            "size", "and", "for", "the", "cutoff", "for", "small", "subfiles"};
        msd_radix_sort(text.begin(), text.end());
        std::cout << text << std::endl;
    }

}
