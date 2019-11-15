#include <iostream>
#include <sstream>

#include "random.h"
#include "array.h"
#include "forward_list.h"
#include "rich_text.h"
#include "box_drawing_chars.h"
#include "string_utils.h"

template<typename It>
void print_sequence(const It& b, const It& e) {
    for (auto el = b; el != e; ++el)
        std::cout << *el << " ";
}

template<typename It>
void buble_sort(It b, It e) {
    int iteration = 0;
    auto print_iteration = [&b, &e, &iteration]() {
        print_sequence(b, e);
        std::cout << "[" << iteration << "]" << std::endl;
        ++iteration;
    };
    print_sequence(b, e);
    std::cout << std::endl;

    auto ee = e;
    for (auto it = b; it != e; ++it) {
        auto previous = b;
        auto current = previous;
        ++current;
        if (current != ee)
            do {
                if (current->value_ < previous->value_)
                    std::swap(current->value_, previous->value_);
                previous = current;
                ++current;
            } while (current != ee);
        ee = previous;
        ee->bold_ = true;
        print_iteration();
    }
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

    std::cout << "array sort" << std::endl;
    buble_sort(array.begin(), array.end());

    std::cout << "forward list sort" << std::endl;
    buble_sort(list.begin(), list.end());
}
