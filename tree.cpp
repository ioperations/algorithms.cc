#include <cstddef>
#include <iostream>

#include "array.h"

template<typename T>
struct Node {
    T data_;
    Array<Node> children_;
    Node(T data, Array<Node>&& children) :data_(data), children_(std::move(children)) {}
    Node(T data) :data_(data) {}
    Node() = default;

    Node(Node&& o) :data_(o.data_), children_(std::move(o.children_)) {}
    Node& operator=(Node&& o) {
        std::swap(data_, o.data_);
        std::swap(children_, o.children_);
        return *this;
    }
};

int main() {
    using node = Node<int>;
    using nodes = Array<node>;

    node n(1, nodes::build_array(
            node(11, nodes::build_array(111, 112)),
            node(12, nodes::build_array(121, 122))
            ));
}
