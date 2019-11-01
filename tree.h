#pragma once

#include <iostream>

#include "array.h"

template<typename T>
class Node {
    private:
        T value_;
        Array<Node> children_;

        static int calculate_depth(const Node<T>& node, int level = 0) {
            ++level;
            int depth = level;
            for (auto it = node.children_.cbegin(); it != node.children_.cend(); ++it)
                depth = std::max(depth, calculate_depth(*it, level));
            return depth;
        }
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
        int depth() const { return calculate_depth(*this); }
};

// #include "tree_printer.h"

// template<typename T>
// std::ostream& operator<<(std::ostream& stream, const Node<T>& node) {
//     Tree_printer().print<Node<T>>(node, stream);
//     return stream;
// }

