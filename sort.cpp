#include <iostream>
#include <sstream>

#include "random.h"
#include "array.h"
#include "forward_list.h"
#include "rich_text.h"
#include "box_drawing_chars.h"
#include "string_utils.h"

template<typename It>
void print_sequence(const It& begin, const It& end) {
    for (auto current = begin; current != end; ++current)
        std::cout << *current << " ";
}

template<typename It>
void buble_sort(It begin, It end) {
    int iteration = 0;
    auto print_iteration = [&begin, &end, &iteration]() {
        print_sequence(begin, end);
        std::cout << "[" << iteration << "]" << std::endl;
        ++iteration;
    };
    print_sequence(begin, end);
    std::cout << std::endl;

    auto end_in = end;
    for (auto current = begin; current != end; ++current) {
        auto previous_in = begin;
        auto current_in = previous_in;
        ++current_in;
        if (current_in != end_in)
            do {
                if (current_in->value_ < previous_in->value_)
                    std::swap(current_in->value_, previous_in->value_);
                previous_in = current_in;
                ++current_in;
            } while (current_in != end_in);
        end_in = previous_in;
        end_in->bold_ = true;
        print_iteration();
    }
}

template<typename It>
void selection_sort(It begin, It end) {
    print_sequence(begin, end);
    std::cout << std::endl;

    auto swap = [&begin, &end](auto current, auto min) {
        std::swap(*current, *min);
        current->bold_ = true;  min->bold_ = true;
        print_sequence(begin, end);
        std::cout << std::endl;
        current->bold_ = false; min->bold_ = false;
    };

    for (auto current = begin; current != end; ++current) {
        auto current_in = current;
        ++current_in;
        if (current_in != end) {
            auto min = current;
            for (; current_in != end; ++current_in)
                if (current_in->value_ < min->value_)
                    min = current_in;
            swap(current, min);
        }
    }
    print_sequence(begin, end);
    std::cout << std::endl;
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
}
