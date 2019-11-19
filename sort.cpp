#include <iostream>
#include <sstream>

#include "random.h"
#include "array.h"
#include "forward_list.h"
#include "rich_text.h"
#include "box_drawing_chars.h"
#include "string_utils.h"
#include "stopwatch.h"

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
class Iteration_printer {
    private:
        const It begin_;
        const It end_;
        const bool verbose_;
        int iteration_ = -1;

        void do_print() {
            for (auto current = begin_; current != end_; ++current)
                std::cout << *current << " ";
            std::cout << "[" << ++iteration_ << "]" << std::endl;
        }
        template<typename F, typename T, typename... Args>
            void for_each(const Style& style, F f, T&& t, Args&&... args) {
                f(t, style);
                for_each(style, f, std::forward<Args>(args)...);
            }
        template<typename F>
        void for_each(const Style& style, F f) {}

    public:
        Iteration_printer(const It& begin, const It& end, bool verbose) 
            :begin_(begin), end_(end), verbose_(verbose) 
        {}

        void print() {
            if (verbose_)
                do_print();
        }
        template<typename... Args>
            void print_bold(Args&&... args) {
                if (verbose_) {
                    for_each(Style::bold(), [](auto& t, auto& style) { t.add_style(style); },
                              std::forward<Args>(args)...);
                    do_print();
                    for_each(Style::bold(), [](auto& t, auto& style) { t.remove_style(style); },
                              std::forward<Args>(args)...);
                }
            }
        template<typename F>
            void print_bold_for_each(F f) {
                if (verbose_) {
                    f(true); do_print(); f(false);
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
        ip.print_bold(*end_in);
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
            ip.print_bold(*current, *min);
        }
    }
}

template<typename It>
void insertion_sort(const It& begin, const It& end, bool verbose = true) {
    Iteration_printer ip(begin, end, verbose);

    for (auto current = end - 1; current != begin; --current)
        compare_and_swap(*current, *(current - 1));
    ip.print_bold(*begin);

    for (auto current = begin + 2; current != end; ++current) {
        auto current_in = current;
        auto value = std::move(*current);
        while (value < *(current_in - 1)) {
            *current_in = std::move(*(current_in - 1));
            --current_in;
        }
        *current_in = std::move(value);

        ip.print_bold_for_each([&current, & current_in](bool bold) {
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

            ip.print_bold(array[i - h], array[j]);
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
        ip.print_bold(*i, *j);
        if (i >= j) break;
        std::swap(*i, *j);
    }
    std::swap(*i, *last);
    return i;
}

template<typename It>
void quick_sort(const It& begin, const It& end, bool verbose = true) {
    Iteration_printer<It> ip(begin, end, verbose);
    ip.print();
    do_quick_sort(begin, end, ip);
}

template<typename It>
void do_quick_sort(const It& begin, const It& end, Iteration_printer<It>& ip) {
    if (end <= begin) return;
    auto i = partition(begin, end, ip);

    i->add_style(Style::red_bg());
    ip.print_bold(*begin, *(end - 1));
    i->remove_style(Style::red_bg());

    do_quick_sort(begin, i, ip);
    do_quick_sort(i + 1, end, ip); 
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
    sorter(container_copy.begin(), container_copy.end(), false);
    std::cout << label << " took " << stopwatch.read_out() << " mls " << std::endl; 
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

    {
        int count = 10'000;
        if (argc > 1)
            count = atoi(argv[1]);
        auto a = Random_sequence_generator(300, 0, count).generate_array<Array<Entry>>();

        sort_and_measure("buble sort", a, buble_sort<Array_iterator>);
        sort_and_measure("selection sort", a, selection_sort<Array_iterator>);
        sort_and_measure("insertion sort", a, insertion_sort<Array_iterator>);
        sort_and_measure("shell sort", a, shell_sort<Array_iterator>);
        sort_and_measure("quick sort", a, quick_sort<Array_iterator>);
    }

}
