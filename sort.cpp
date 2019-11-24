#include <iostream>
#include <sstream>

#include "random.h"
#include "array.h"
#include "forward_list.h"
#include "rich_text.h"
#include "stopwatch.h"
#include "stack.h"
#include "pair.h"

using Style = Rich_text::Style;

template<typename T>
bool compare_and_swap(T& t1, T& t2) {
    if (t1 < t2) {
        std::swap(t1, t2);
        return true;
    }
    return false;
}

template<typename It>
class Iteration_printer : protected Rich_text::Sequence<It> {
    private:
        using Base = Rich_text::Sequence<It>;
        const bool verbose_;
        int iteration_ = -1;

        void print_index() {
            std::cout << " [" << ++iteration_ << "]" << std::endl;
        }
    public:
        Iteration_printer(const It& begin, const It& end, bool verbose) 
            :Base(begin, end), verbose_(verbose)
        {}
        void reset(const It& begin) {
            Base::reset(begin, begin + (Base::end() - Base::begin()));
        }
        template<typename... ES>
            void print_with_styled_entry(const Style& style, ES&... entries) {
                if (verbose_) {
                    Base::print_with_styled_entry(style, entries...);
                    print_index();
                }
            }
        template<typename... SES>
            void print_with_styled_entries(SES&&... styled_entries) {
                if (verbose_) {
                    Base::print_with_styled_entries(std::forward<SES>(styled_entries)...);
                    print_index();
                }
            }
        template<typename F>
            void print(F f) {
                if (verbose_) {
                    f(true);
                    Base::print(std::cout);
                    print_index();
                    f(false);
                }
            }
};

template<typename It>
void buble_sort(const It& begin, const It& end, bool verbose = true) {
    Iteration_printer ip(begin, end, verbose);

    auto end_in = end;
    bool swapped = true;
    for (auto current = begin; current != end && swapped; ++current) {
        swapped = false;
        auto previous_in = begin;
        auto current_in = previous_in;
        for (++current_in; current_in != end_in; ++current_in) {
            swapped = compare_and_swap(*current_in, *previous_in);
            previous_in = current_in;
        }
        end_in = previous_in;
        ip.print_with_styled_entry(Style::bold(), *end_in);
    }
}

template<typename It>
void selection_sort(const It& begin, const It& end, bool verbose = true) {
    Iteration_printer ip(begin, end, verbose);

    for (auto current = begin; current != end; ++current) {
        auto current_in = current;
        ++current_in;
        if (current_in != end) {
            auto min = current;
            for (; current_in != end; ++current_in)
                if (*current_in < *min)
                    min = current_in;
            compare_and_swap(*min, *current);
            ip.print_with_styled_entry(Style::bold(), *current, *min);
        }
    }
}

template<typename It>
void insertion_sort(const It& begin, const It& end, bool verbose = true) {
    Iteration_printer ip(begin, end, verbose);

    for (auto current = end - 1; current != begin; --current)
        compare_and_swap(*current, *(current - 1));
    ip.print_with_styled_entry(Style::bold(), *begin);

    for (auto current = begin + 2; current != end; ++current) {
        auto current_in = current;
        auto value = std::move(*current);
        while (value < *(current_in - 1)) {
            *current_in = std::move(*(current_in - 1));
            --current_in;
        }
        *current_in = std::move(value);

        ip.print([&current, & current_in](bool bold) {
            for (auto current_h = current; current_h != current_in - 1; --current_h)
                current_h->set_style(bold ? Style::bold() : Style::normal());
        });
    }
}

template<typename It>
void shell_sort(const It& begin, const It& end, bool verbose = true) {
    Iteration_printer ip(begin, end, verbose);

    size_t l = 0;
    size_t r = end - begin - 1;
    auto array = begin;

    size_t h;
    for (h = 1; h <= (r - l) / 9; h = 3 * h + 1);
    for ( ; h > 0; h /= 3) {
        for (size_t i = l + h; i <= r; ++i) {
            auto j = i;
            auto v = std::move(array[i]);
            while (j >= l + h && v < array[j - h]) {
                array[j] = std::move(array[j - h]);
                j -= h;
            }
            array[j] = std::move(v);

            ip.print_with_styled_entry(Style::bold(), array[i - h], array[j]);
        }
    }
}

template<typename It>
auto partition(const It& begin, const It& end, Iteration_printer<It>& ip) {
    const auto& last = end - 1;
    auto i = begin - 1, j = last;
    auto& v = *last;
    for (;;) {
        while (*++i < v);
        while (v < *--j && j != begin);
        ip.print_with_styled_entry(Style::bold(), *i, *j);
        if (i >= j) break;
        std::swap(*i, *j);
    }
    std::swap(*i, *last);
    return i;
}

template<typename It>
void quick_sort(const It& begin, const It& end, bool verbose = true) {
    Iteration_printer<It> ip(begin, end, verbose);
    ip.print_with_styled_entries();
    do_quick_sort(begin, end, ip);
}

template<typename It>
void print_quick_sort_partition_points(Iteration_printer<It>& ip, 
                                       const It& begin, const It& end, const It& i) {
    ip.print_with_styled_entries(
        Rich_text::styled_entries(Style::bold(), *begin, *(end - 1)),
        Rich_text::styled_entries(Style::red_bg(), *i));
}

template<typename It>
void do_quick_sort(const It& begin, const It& end, Iteration_printer<It>& ip) {
    if (end <= begin) return;
    auto i = partition(begin, end, ip);

    print_quick_sort_partition_points(ip, begin, end, i);

    do_quick_sort(begin, i, ip);
    do_quick_sort(i + 1, end, ip); 
}

template<typename It>
struct Quick_sort_frame {
    It begin_;
    It end_;
    int depth_;
    Quick_sort_frame(const It& begin, const It& end, int depth) 
        :begin_(begin), end_(end), depth_(depth) 
    {}
    bool operator<(const Quick_sort_frame& o) {
        return end_ - begin_ < o.end_ - o.begin_;
    }
};

template<typename It>
void quick_sort_stack(const It& begin, const It& end, bool verbose = true) {
    Iteration_printer<It> ip(begin, end, verbose);
    ip.print_with_styled_entries();
    Stack<Quick_sort_frame<It>> stack;
    auto push = [&stack](auto interval) {
        if (interval.begin_ < interval.end_)
            stack.push(interval);
    };
    push(Quick_sort_frame<It>(begin, end, 1));
    int max_depth = 0;
    while (!stack.empty()) {
        auto interval = stack.pop();
        auto i = partition(interval.begin_, interval.end_, ip);

        print_quick_sort_partition_points(ip, interval.begin_, interval.end_, i);

        Quick_sort_frame<It> f1(interval.begin_, i, interval.depth_ + 1);
        Quick_sort_frame<It> f2(i + 1, interval.end_, interval.depth_ + 1);
        compare_and_swap(f1, f2);
        push(f2);
        push(f1);
        max_depth = std::max(max_depth, interval.depth_);
    };
    std::cout << "stack depth: " << max_depth << std::endl;
}

template<typename It>
void median_of_three_quick_sort(const It& begin, const It& end, Iteration_printer<It>& ip) {
    if (end - begin > 11) {
        ip.print_with_styled_entries();
        std::swap(*(begin + (end - begin) / 2), *(end - 2));
        compare_and_swap(*begin, *(end - 2));
        compare_and_swap(*begin, *(end - 1));
        compare_and_swap(*(end - 1), *(end - 2));

        auto i = partition(begin + 1, end, ip);
        median_of_three_quick_sort(begin, i, ip);
        median_of_three_quick_sort(i + 1, end, ip);
    }
}

template<typename It>
void hybrid_sort(const It& begin, const It& end, bool verbose = false) {
    Iteration_printer ip(begin, end, verbose);
    median_of_three_quick_sort(begin, end, ip);
    insertion_sort(begin, end, verbose);
}

template<typename It>
void non_recursive_hybrid_sort(const It& begin, const It& end, bool verbose = false) {
    Iteration_printer ip(begin, end, verbose);
    Stack<Quick_sort_frame<It>> stack;
    auto push = [&stack](auto f) {
        if (f.end_ - f.begin_ > 11)
            stack.push(f);
    };
    push(Quick_sort_frame<It>(begin, end, 1));
    int max_depth = 0;
    while (!stack.empty()) {
        ip.print_with_styled_entries();
        auto interval = stack.pop();
        auto first = interval.begin_ + 1;
        auto middle = interval.begin_ + (interval.end_ - interval.begin_) / 2;
        auto last_but_one = interval.end_ - 2;
        auto last = interval.end_ - 1;
        auto print_ = [&ip, &first, &middle, &last_but_one, &last]() {
            ip.print_with_styled_entry(Style::red_bg(), *first, *middle,
                                       *last_but_one, *last);
        };
        print_();

        std::swap(*middle, *last_but_one);
        compare_and_swap(*interval.begin_, *last_but_one);
        compare_and_swap(*interval.begin_, *last);
        compare_and_swap(*last, *last_but_one);
        print_();

        auto i = partition(first, interval.end_, ip);
        print_quick_sort_partition_points(ip, first, last, i);
        Quick_sort_frame<It> f1(interval.begin_, i, interval.depth_ + 1);
        Quick_sort_frame<It> f2(i + 1, interval.end_, interval.depth_ + 1);
        compare_and_swap(f1, f2);
        push(f2);
        push(f1);
        max_depth = std::max(max_depth, interval.depth_);
    }
    std::cout << "stack depth: " << max_depth << std::endl;
    insertion_sort(begin, end, verbose);
}

template<typename C, typename S>
void sort(std::string&& label, const C& container, S sorter) {
    std::cout << label << " sort" << std::endl;
    auto container_copy = container;
    sorter(container_copy.begin(), container_copy.end(), true);
}

template<typename C, typename S>
void sort_and_measure(std::string&& label, const C& container, S sorter) {
    auto container_copy = container;
    Stopwatch stopwatch;
    auto sort = [&sorter, &container_copy, &stopwatch](const std::string& label) {
        sorter(container_copy.begin(), container_copy.end(), false);
        std::cout << label << " took " << stopwatch.read_out() << " mls " << std::endl; 
    };
    sort(label);
    sort(label + " (sorted)");
}

template<typename It>
void merge(const It& b, const It& m, const It& e, const It& b_aux) {
    using index_type = decltype(e - b);
    index_type i, j, r = e - b - 1, middle = m - b;
    for (i = middle + 1; i > 0; --i)
        *(b_aux + i - 1) = std::move(*(b + i - 1));
    for (j = middle; j < r; ++j)
        *(b_aux + r + middle - j) = std::move(*(b + j + 1));

    for (auto it = b; it != e; ++it)
        if (*(b_aux + j) < *(b_aux + i))
            *it = std::move(*(b_aux + j--));
        else
            *it = std::move(*(b_aux + i++));
}

template<typename It>
void do_merge_sort(const It& b, const It& e, const It& b_aux, Iteration_printer<It>& ip) {
    if (e > b + 1) {
        auto m = b + ((e - b - 1) / 2);
        print_quick_sort_partition_points(ip, b, e, m);
        do_merge_sort(b, m + 1, b_aux, ip);
        do_merge_sort(m + 1, e, b_aux, ip);
        merge(b, m, e, b_aux);
    }
}

template<typename It>
void merge_sort(const It& b, const It& e, bool verbose = false) {
    Array<typename std::iterator_traits<It>::value_type> aux(e - b);
    Iteration_printer ip(b, e, verbose);
    do_merge_sort(b, e, aux.begin(), ip);
    ip.print_with_styled_entries();
}

template<typename It>
void print_seq(const It& b, const It& e) {
    for (auto item = b; item != e; ++item)
        std::cout << *item << " ";
    std::cout << std::endl;
}

template<typename It, typename In>
void stable_merge(const It& b, const It& b_aux, In l, In m, In r) {
    In i = l, j = m + 1;
    for (In k = l; k <= r; ++k) {
        auto add = [&](auto& index) { *(b + k) = std::move(*(b_aux + index++)); };
        if (i == m + 1)
            add(j);
        else if (j == r + 1)
            add(i);
        else if (*(b_aux + j) < *(b_aux + i))
            add(j);
        else
            add(i);
    }
}

template<typename It, typename In>
void do_stable_merge_sort(const It& b, const It& b_aux, In l, In r,
                          Iteration_printer<It>& ip) {
    if (l < r) {
        In m = (l + r) / 2;
        ip.reset(b);
        print_quick_sort_partition_points(ip, b + l, b + r + 1, b + m);
        do_stable_merge_sort(b_aux, b, l, m, ip);
        do_stable_merge_sort(b_aux, b, m + 1, r, ip);
        stable_merge(b, b_aux, l, m, r);
    }
}

template<typename It>
void stable_merge_sort(const It& b, const It& e, bool verbose = false) {
    Array<typename std::iterator_traits<It>::value_type> aux(e - b);
    for (decltype(e - b) i = 0; i < e - b; ++i)
        aux[i] = std::move(*(b + i));
    Iteration_printer ip(b, e, verbose);
    ip.print_with_styled_entries();
    do_stable_merge_sort(b, aux.begin(), static_cast<decltype(e - b)>(0), e - b - 1, ip);
    ip.print_with_styled_entries();
}

template<typename It>
void non_recursive_merge_sort(const It& b, const It& e, bool verbose = false) {
    Array<typename std::iterator_traits<It>::value_type> aux(e - b);
    for (decltype(e - b) i = 0; i < e - b; ++i)
        aux[i] = std::move(*(b + i));
    auto bb = b;
    auto b_aux = aux.begin();

    Iteration_printer ip(b, e, verbose);

    using index_type = decltype(e - b);
    auto r = e - b - 1;
    for (index_type step = 2; step <= r; step *= 2) {
        for (index_type l = 0; l <= r; l += step) {
            auto rr = std::min(r, l + step - 1);
            auto m = (l + rr) / 2;
            print_quick_sort_partition_points(ip, bb + l, bb + rr + 1, bb + m);
            stable_merge(bb, b_aux, l, m, rr);
        }
        std::swap(bb, b_aux);
        ip.reset(bb);
    }
    ip.reset(b);
    ip.print_with_styled_entries();
}

int main(int argc, const char** argv) {
    using Entry = Rich_text::Entry<int>;
    Array<Entry> array(15);
    Forward_list<Entry> list;
    {
        Random_sequence_generator generator(300, 10, 99);
        for (auto& e : array) {
            int value = generator.generate();
            e.value_ = value;
            list.emplace_back(value);
        }
    }

    using Array_iterator = Array<Entry>::iterator;
    using List_iterator = Forward_list<Entry>::iterator;

    sort("array bubble", array, buble_sort<Array_iterator>);
    sort("forward list bubble", list, buble_sort<List_iterator>);

    sort("array selection", array, selection_sort<Array_iterator>);
    sort("forward list selection", list, selection_sort<List_iterator>);

    sort("array insertion", array, insertion_sort<Array_iterator>);
    sort("array shell", array, shell_sort<Array_iterator>);
    sort("array quick", array, quick_sort<Array_iterator>);
    sort("array stack quick", array, quick_sort_stack<Array_iterator>);
    sort("hybrid", array, hybrid_sort<Array_iterator>);
    sort("non-recursive hybrid", array, non_recursive_hybrid_sort<Array_iterator>);
    sort("merge", array, merge_sort<Array_iterator>);
    sort("stable merge", array, stable_merge_sort<Array_iterator>);
    sort("non-recursive merge", array, non_recursive_merge_sort<Array_iterator>);

    {
        int count = 10'000;
        if (argc > 1)
            count = atoi(argv[1]);
        std::cout << "items cout: " << count << std::endl;
        auto a = Random_sequence_generator(300, 0, count).generate_array<Array<Entry>>();

        sort_and_measure("buble sort", a, buble_sort<Array_iterator>);
        sort_and_measure("selection sort", a, selection_sort<Array_iterator>);
        sort_and_measure("insertion sort", a, insertion_sort<Array_iterator>);
        sort_and_measure("shell sort", a, shell_sort<Array_iterator>);
        sort_and_measure("quick sort", a, quick_sort<Array_iterator>);
        sort_and_measure("stack quick sort", a, quick_sort_stack<Array_iterator>);
        sort_and_measure("hybrid sort", a, hybrid_sort<Array_iterator>);
        sort_and_measure("non-recursive hybrid sort", a, non_recursive_hybrid_sort<Array_iterator>);
        sort_and_measure("merge sort", a, merge_sort<Array_iterator>);
        sort_and_measure("stable merge sort", a, stable_merge_sort<Array_iterator>);
        sort_and_measure("non-recursive merge sort", a, non_recursive_merge_sort<Array_iterator>);
    }

}
