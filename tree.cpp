
#include <cstddef>
#include <iostream>

#include "array.h"

template<typename T>
struct Node {
    T data_;
    Node* children_;
    size_t children_size_;
    Node(T data, Node* children, size_t children_size) :data_(data), children_(children), children_size_(children_size) {}
    ~Node() { delete [] children_; }
};

int main() {
    List<int> list;
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    for (auto& t : list)
        std::cout << t << std::endl;

    auto a = Array<int>::Builder().add(1).add(2).add(3).build();

    for (auto& t : a)
        std::cout << t << std::endl;

}
