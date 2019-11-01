#pragma once

#include <iostream>

#include "array.h"

template<typename T>
class Node {
    private:
        T value_;
        Array<Node> children_;

        template<typename TT>
            static int calculate_depth(Node<TT>& node, int level = 0) {
                ++level;
                int depth = level;
                for (auto& child : node.children())
                    depth = std::max(depth, calculate_depth(child, level));
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

        T value() { return value_; }
        Array<Node>& children() { return children_; }
        int depth() { return calculate_depth(*this); }
};

#include "tree_printer.h"

template<typename T>
std::ostream& operator<<(std::ostream& stream, const Node<T>& node) {
    // Tree_printer().print(node, stream);
    return stream;
}

