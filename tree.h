#pragma once

#include <iostream>

#include "array.h"
#include "tree_printer.h"

template<typename T>
class Tree_node {
    private:
        T value_;
        Array<Tree_node> children_;
    public:
        Tree_node(T data, Array<Tree_node>&& children) :value_(data), children_(std::move(children)) {}
        Tree_node(T data) :value_(data) {}
        Tree_node() = default;

        Tree_node(const Tree_node&) = delete;
        Tree_node& operator=(const Tree_node&) = delete;

        Tree_node(Tree_node&& o) :value_(o.value_), children_(std::move(o.children_)) {}
        Tree_node& operator=(Tree_node&& o) {
            std::swap(value_, o.value_);
            std::swap(children_, o.children_);
            return *this;
        }

        T value() const { return value_; }
        const Array<Tree_node>& children() const { return children_; }
};

template<typename T>
std::ostream& operator<<(std::ostream& stream, const Tree_node<T>& node) {
    Tree_printer::default_instance().print(node, stream);
    return stream;
}

