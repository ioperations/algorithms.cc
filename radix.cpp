#include <bitset>
#include <climits>
#include <iostream>

#include "array.h"
#include "random.h"
#include "stack.h"

template <typename It>
void print_seq(const It& begin, const It& end) {
    for (auto el = begin; el != end; ++el) std::cout << *el << " ";
    std::cout << std::endl;
}

template <typename T>
inline unsigned char get_bit(T t, unsigned int digit) {
    return (t & (T(1) << digit)) > 0 ? 1 : 0;
}

template <typename It>
void radix_quick_sort(const It& begin, const It& end) {
    using value_type = typename std::iterator_traits<It>::value_type;

    auto print_bits = [begin, end]() {
        for (auto el = begin; el != end; ++el)
            std::cout << std::bitset<CHAR_BIT * sizeof(value_type)>(*el)
                      << std::endl;
    };

    struct Interval {
        It m_begin;
        It m_end;
        unsigned int m_digit;
        Interval(It begin, It end, unsigned int digit)
            : m_begin(begin), m_end(end), m_digit(digit) {}
    };
    Stack<Interval> stack;

    stack.emplace(begin, end, sizeof(value_type) * CHAR_BIT - 1);

    print_seq(begin, end);
    print_bits();
    while (!stack.empty()) {
        auto interval = stack.pop();
        if (interval.m_begin < interval.m_end && interval.m_digit > 0) {
            auto i = interval.m_begin;
            auto j = interval.m_end - 1;
            while (i != j) {
                while (get_bit(*i, interval.m_digit) == 0 && i < j) ++i;
                while (get_bit(*j, interval.m_digit) == 1 && j > i) --j;
                std::swap(*i, *j);
            }
            if (get_bit(*(interval.m_end - 1), interval.m_digit) == 0) ++j;
            stack.emplace(j, interval.m_end, interval.m_digit - 1);
            stack.emplace(interval.m_begin, j, interval.m_digit - 1);
        }
    }
    print_seq(begin, end);
    print_bits();
}

template <typename It>
void index_sort(const It& begin, const It& end) {
    if (begin == end) return;
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
    for (auto& position : positions) position = 0;
    for (auto el = begin; el != end; ++el) positions[*el - min + 1]++;
    std::cout << "counts: " << std::endl << positions << std::endl;
    for (size_t i = 1; i < positions.size(); ++i)
        positions[i] += positions[i - 1];
    std::cout << "positions: " << std::endl << positions << std::endl;
    Array<int> aux(end - begin);
    for (long int i = 0; i < end - begin; ++i)
        aux[positions[begin[i] - min]++] = begin[i];
    for (long int i = 0; i < end - begin; ++i) begin[i] = aux[i];
}

template <typename It>
void msd_radix_sort(const It& begin, const It& end) {
    if (begin == end) return;
    Array<unsigned int> positions(UCHAR_MAX + 1);
    Array<std::string> aux(end - begin);

    struct Frame {
        const It m_begin;
        const It m_end;
        const size_t m_char_index;
        Frame(const It& begin, const It& end, const size_t char_index)
            : m_begin(begin), m_end(end), m_char_index(char_index) {}
    };
    Stack<Frame> stack;
    stack.emplace(begin, end, 0);

    while (!stack.empty()) {
        auto f = stack.pop();

        for (auto& e : positions) e = 0;
        auto char_of = [char_index =
                            f.m_char_index](std::string& s) -> unsigned char {
            return char_index < s.size() ? s[char_index] : 0;
        };
        for (auto s = f.m_begin; s != f.m_end; ++s)
            ++positions[char_of(*s) + 1];
        for (size_t i = 1; i < positions.size(); ++i)
            positions[i] += positions[i - 1];

        size_t max_length = 0;
        for (long int i = 0; i < f.m_end - f.m_begin; ++i) {
            std::string& str = f.m_begin[i];
            max_length = std::max(max_length, str.size());
            auto& pos = positions[char_of(str)];
            aux[pos] = std::move(f.m_begin[i]);
            ++pos;
        }
        for (long i = 0; i < f.m_end - f.m_begin; ++i) f.m_begin[i] = aux[i];

        if (f.m_char_index + 1 < max_length)
            for (long b = 0; b < f.m_end - f.m_begin;) {
                auto e = positions[f.m_begin[b][f.m_char_index]];
                if (e - b > 1)
                    stack.emplace(f.m_begin + b, f.m_begin + e,
                                  f.m_char_index + 1);
                b = e;
            }
    }
}

template <typename It>
void quick_sort_three_part(const It& begin, const It& end) {
    std::cout << std::endl;
    auto print = [begin, end]() {
        for (auto el = begin; el != end; ++el) std::cout << *el << " ";
        std::cout << std::endl;
    };
    print();

    struct Frame {
        const It m_begin;
        const It m_end;
        Frame(const It& begin, const It& end) : m_begin(begin), m_end(end) {}
    };
    struct : public Stack<Frame> {
        void emplace_if_required(const It& begin, const It& end) {
            if (end - begin > 1) Stack<Frame>::emplace(begin, end);
        }
    } stack;
    stack.emplace_if_required(begin, end);

    while (!stack.empty()) {
        auto f = stack.pop();
        auto i = f.m_begin - 1, p = f.m_begin - 1, j = f.m_end - 1,
             q = f.m_end - 1;
        auto v = *(f.m_end - 1);

        while (true) {
            while (*(++i) < v)
                ;
            while (*(--j) > v)
                if (j == f.m_begin) break;
            if (i >= j) break;
            std::swap(*i, *j);
            if (*i == v) std::swap(*i, *(++p));
            if (*j == v) std::swap(*j, *(--q));
        }
        std::swap(*i, *(f.m_end - 1));
        j = i;

        for (auto el = f.m_begin; el <= p; ++el) std::swap(*(--i), *el);
        for (auto el = q; el != f.m_end - 1; ++el) std::swap(*(++j), *el);

        stack.emplace_if_required(f.m_begin, i);
        stack.emplace_if_required(j + 1, f.m_end);
    }
}

int main() {
    {
        auto array = RandomSequenceGenerator<int>(300, 10, 99)
                         .generate_array<Array<int>>(15);
        std::cout << array << std::endl;

        Array<int> ids(100);
        for (size_t i = 0; i < array.size(); ++i) ids[array[i]] = array[i];
        std::cout << ids << std::endl;
        auto array_copy = array;
        index_sort(array_copy.begin(), array_copy.end());
        std::cout << array_copy << std::endl;
    }
    {
        Array<int> array{50, 29, 29, 46, 58, 43, 24, 36, 38, 31,
                         73, 80, 48, 82, 99, 46, 48, 82, 99, 46};
        std::cout << array.size() << std::endl;
        quick_sort_three_part(array.begin(), array.end());
    }
    {
        using type = unsigned char;
        RandomSequenceGenerator<type> generator(400, 'A', 'Z');
        auto array = generator.generate_array<Array<type>>(15);
        radix_quick_sort(array.begin(), array.end());
    }
    {
        Array<std::string> text{
            "to",          "achieve", "good",     "performance", "using",
            "radix",       "sort",    "for",      "a",           "particular",
            "application", "we",      "need",     "to",          "limit",
            "the",         "number",  "of",       "empty",       "bins",
            "encountered", "by",      "choosing", "appropriate", "values",
            "both",        "for",     "the",      "radix",       "size",
            "and",         "for",     "the",      "cutoff",      "for",
            "small",       "subfiles"};
        msd_radix_sort(text.begin(), text.end());
        std::cout << text << std::endl;
    }
}
