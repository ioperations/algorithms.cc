#include <iostream>
#include <sstream>

#include "random.h"
#include "array.h"
#include "forward_list.h"
#include "rich_text.h"
#include "box_drawing_chars.h"
#include "string_utils.h"

template<typename It>
struct Printable_sequence {
    It begin_;
    It end_;
    Printable_sequence(const It& begin, const It& end) :begin_(begin), end_(end) {}
};
template<typename It>
std::ostream& operator<<(std::ostream& stream, const Printable_sequence<It>& p) {
    for (auto current = p.begin_; current != p.end_; ++current)
        std::cout << *current << " ";
    return stream;
}

template<typename It>
void buble_sort(const It& begin, const It& end) {
    auto ps = Printable_sequence(begin, end);
    int iteration = 0;
    auto print_iteration = [&ps, &iteration]() {
        std::cout << ps << "[" << iteration << "]" << std::endl;
        ++iteration;
    };
    std::cout << ps << std::endl;

    auto end_in = end;
    for (auto current = begin; current != end; ++current) {
        auto previous_in = begin;
        auto current_in = previous_in;
        ++current_in;
        if (current_in != end_in)
            do {
                if (*current_in < *previous_in)
                    std::swap(*current_in, *previous_in);
                previous_in = current_in;
                ++current_in;
            } while (current_in != end_in);
        end_in = previous_in;
        end_in->bold_ = true;
        print_iteration();
    }
}

template<typename It>
void selection_sort(const It& begin, const It& end) {
    auto ps = Printable_sequence(begin, end);
    std::cout << ps << std::endl;

    auto swap = [&ps](auto current, auto min) {
        std::swap(*current, *min);
        current->bold_ = true;  min->bold_ = true;
        std::cout << ps << std::endl;
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
            swap(current, min);
        }
    }
    std::cout << ps << std::endl;
}

template<typename It>
void insertion_sort(const It& begin, const It& end) {
    auto ps = Printable_sequence(begin, end);
    std::cout << ps << std::endl;

    for (auto current = end - 1; current != begin; --current)
        if (*current < *(current - 1))
            std::swap(*(current - 1), *current);

    for (auto current = begin + 2; current != end; ++current) {
        auto current_in = current;
        while (*current < *(current_in - 1)) {
            *current_in = std::move(*(current_in - 1));
            --current_in;
        }
        *current_in = *current;
    }
    std::cout << ps << std::endl;
}

using Entry = Rich_text<int>;

int main() {
    Random_sequence_generator generator(300, 10, 99);

    Array<Entry> array(15);
    Forward_list<Entry> list;
    for (auto& e : array) {
        int value = generator.generate();
        e.value_ = value;
        list.emplace_back(value, false);
    }

    std::cout << "array bubble sort" << std::endl;
    auto array_copy = array;
    buble_sort(array_copy.begin(), array_copy.end());

    std::cout << "forward list bubble sort" << std::endl;
    auto list_copy = list;
    buble_sort(list_copy.begin(), list_copy.end());

    std::cout << "array selection sort" << std::endl;
    array_copy = array;
    selection_sort(array_copy.begin(), array_copy.end());

    std::cout << "forward list selection sort" << std::endl;
    list_copy = list;
    selection_sort(list_copy.begin(), list_copy.end());

    std::cout << "array insertion sort" << std::endl;
    array_copy = array;
    insertion_sort(array_copy.begin(), array_copy.end());
}
