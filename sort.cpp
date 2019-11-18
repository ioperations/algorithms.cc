#include <iostream>
#include <sstream>
#include <chrono>

#include "random.h"
#include "array.h"
#include "forward_list.h"
#include "rich_text.h"
#include "box_drawing_chars.h"
#include "string_utils.h"

template<typename T>
bool compare_and_swap(T& t1, T& t2) {
    if (t1 < t2) {
        std::swap(t1, t2);
        return true;
    }
    return false;
}

template<typename It>
struct Iteration_printer {
    const It begin_;
    const It end_;
    const bool verbose_;
    int iteration_ = -1;
    Iteration_printer(const It& begin, const It& end, bool verbose) :begin_(begin), end_(end), verbose_(verbose) {}
    void print() {
        if (verbose_) {
            for (auto current = begin_; current != end_; ++current)
                std::cout << *current << " ";
            std::cout << "[" << ++iteration_ << "]" << std::endl;
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
        end_in->bold_ = true;
        ip.print();
    }
}

template<typename It>
void selection_sort(const It& begin, const It& end, bool verbose = true) {
    Iteration_printer ip(begin, end, verbose);

    auto print_iteration = [&ip](auto current, auto min) {
        current->bold_ = true;  min->bold_ = true;
        ip.print();
        current->bold_ = false; min->bold_ = false;
    };

    for (auto current = begin; current != end; ++current) {
        auto current_in = current;
        ++current_in;
        if (current_in != end) {
            auto min = current;
            for (; current_in != end; ++current_in)
                if (*current_in < *min)
                    min = current_in;
            compare_and_swap(*min, *current);
            print_iteration(current, min);
        }
    }
}

template<typename It>
void insertion_sort(const It& begin, const It& end, bool verbose = true) {
    Iteration_printer ip(begin, end, verbose);

    for (auto current = end - 1; current != begin; --current)
        compare_and_swap(*current, *(current - 1));
    ip.print();

    for (auto current = begin + 2; current != end; ++current) {
        auto current_in = current;
        auto value = std::move(*current);
        while (value < *(current_in - 1)) {
            *current_in = std::move(*(current_in - 1));
            --current_in;
        }
        *current_in = std::move(value);

        auto set_bold = [&current, & current_in](bool bold) {
            for (auto current_h = current; current_h != current_in - 1; --current_h)
                current_h->bold_ = bold;
        };
        set_bold(true); ip.print(); set_bold(false);
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

            array[i - h].bold_ = true;
            array[j].bold_ = true;
            ip.print();
            array[i - h].bold_ = false;
            array[j].bold_ = false;
        }
    }
}

using Entry = Rich_text<int>;

template<typename C, typename S>
void sort(const C& container, S sorter) {
    auto container_copy = container;
    sorter(container_copy.begin(), container_copy.end(), true);
}

long now() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
        .count();
}

template<typename C, typename S>
auto sort_and_measure(const C& container, S sorter) {
    auto container_copy = container;
    auto ts = now();
    sorter(container_copy.begin(), container_copy.end(), false);
    return now() - ts;
}

template<typename A>
size_t partition(A& a, size_t l, size_t r) {
    size_t i = l - 1, j = r;
    auto& v = a[r];
    for (;;) {
        while (a[++i] < v);
        while (v < a[--j])
            if (j == l) break;
        if (i >= j) break;
        std::swap(a[i], a[j]);
    }
    std::swap(a[i], a[r]);
    return i;
}

template<typename A>
void quick_sort(A& a, size_t l, size_t r) {
    std::cout << a << std::endl;
    if (r <= l) return;
    auto i = partition(a, l, r);
    {
        auto ii = i - 1;
        if (ii < i)
            quick_sort(a, l, ii);
    }
    quick_sort(a, i + 1, r); 
}

int main(int argc, const char** argv) {
    Array<Entry> array(15);
    Forward_list<Entry> list;
    {
        Random_sequence_generator generator(300, 10, 99);
        for (auto& e : array) {
            int value = generator.generate();
            e.value_ = value;
            list.emplace_back(value, false);
        }
    }

    using Array_iterator = Array<Entry>::iterator;
    using List_iterator = Forward_list<Entry>::iterator;

    std::cout << "array bubble sort" << std::endl;
    sort(array, buble_sort<Array_iterator>);

    std::cout << "forward list bubble sort" << std::endl;
    sort(list, buble_sort<List_iterator>);

    std::cout << "array selection sort" << std::endl;
    sort(array, selection_sort<Array_iterator>);

    std::cout << "forward list selection sort" << std::endl;
    sort(list, selection_sort<List_iterator>);

    std::cout << "array insertion sort" << std::endl;
    sort(array, insertion_sort<Array_iterator>);

    std::cout << "array shell sort" << std::endl;
    sort(array, shell_sort<Array_iterator>);

    quick_sort(array, 0, array.size() - 1);
    std::cout << array << std::endl;

    {
        int count = 10'000;
        if (argc > 1)
            count = atoi(argv[1]);

        Random_sequence_generator generator(300, 0, count);
        auto a = generator.generate_array<Array<Entry>>(count);

        auto sort_measure_print = [](const std::string& label, auto mls) {
            std::cout << label << " took " << mls << "mls " << std::endl; };

        sort_measure_print("buble sort", sort_and_measure(a, buble_sort<Array_iterator>));
        sort_measure_print("selection sort", sort_and_measure(a, selection_sort<Array_iterator>));
        sort_measure_print("insertion sort", sort_and_measure(a, insertion_sort<Array_iterator>));
        sort_measure_print("shell sort", sort_and_measure(a, shell_sort<Array_iterator>));
    }
}
