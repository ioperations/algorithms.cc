#include <random>
#include <iostream>

#include "array.h"
#include "forward_list.h"
#include "rich_text.h"

class Random_sequence_generator {
    private:
        std::mt19937 engine;
        std::uniform_int_distribution<std::mt19937::result_type> distribution;
    public:
        Random_sequence_generator(unsigned long seed, int b, int e)
            :engine(seed), distribution(b, e)
        {}
        Array<int> generate_array(size_t size) {
            Array<int> array(size);
            for (auto& e : array)
                e = generate();
            return array;
        }
        int generate() {
            return distribution(engine);
        }
};

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

template<typename It>
void sequential_search(It b, It e, int value) {
    std::cout << "searching for " << value << std::endl;
    int index = -1;
    int i = 0;
    for (auto el = b; index == -1 && el != e; ++el, ++i) {
        el->bold_ = true;
        if (el->value_ == value)
            index = i;
    }
    print_sequence(b, e);
    std::cout << std::endl << "index = " << index << std::endl;
}

using Entry = Rich_text<int>;

void quick_search(Array<Entry>& array, int value) {
    std::cout << "searching for " << value << std::endl;
    int i = 0;
    int j = array.size() - 1;
    int index = -1;
    while (index == -1 && i < j) {
        auto middle = i + (j - i) / 2;
        array[middle].bold_ = true;
        print_sequence(array.cbegin(), array.cend());
        array[middle].bold_ = false;
        std::cout << std::endl;
        if (value == array[middle].value_)
            index = middle;
        else if (value < array[middle].value_)
            j = middle;
        else
            i = middle + 1;
    }
    std::cout << "index: " <<  ((int) index) << std::endl;
}

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

    std::cout << "array sequential search" << std::endl;
    clear_rich_text_format(array.begin(), array.end());
    sequential_search(array.cbegin(), array.cend(), 58);

    std::cout << "list sequential search" << std::endl;
    clear_rich_text_format(list.begin(), list.end());
    sequential_search(list.begin(), list.end(), 58);

    std::cout << "array quick search" << std::endl;
    auto do_quick_search = [&array](int value) {
        clear_rich_text_format(array.begin(), array.end());
        quick_search(array, value);
    };
    do_quick_search(58);
    do_quick_search(12);
    do_quick_search(100);
}
