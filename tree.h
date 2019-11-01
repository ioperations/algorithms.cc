#pragma once

#include <iostream>

#include "array.h"
#include "tree_printer.h"

template<typename T>
class Node {
    private:
        T value_;
        Array<Node> children_;
    public:
        Node(T data, Array<Node>&& children) :value_(data), children_(std::move(children)) {}
        Node(T data) :value_(data) {}
        Node() = default;

        Node(Node&& o) :value_(o.value_), children_(std::move(o.children_)) {}
        Node& operator=(Node&& o) {
            std::swap(value_, o.value_);
            std::swap(children_, o.children_);
            return *this;
        }

        T value() const { return value_; }
        const Array<Node>& children() const { return children_; }
};

template<typename T>
std::ostream& operator<<(std::ostream& stream, const Node<T>& node) {
    Tree_printer::default_instance().print(node, stream);
    return stream;
}

