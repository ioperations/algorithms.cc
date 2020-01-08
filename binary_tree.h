#pragma once

#include <iostream>

#include "tree_printer.h"

template<typename T>
struct Binary_tree_node {
    T value_;
    Binary_tree_node* l_;
    Binary_tree_node* r_;
    Binary_tree_node(T value, Binary_tree_node* l = nullptr, Binary_tree_node* r = nullptr) 
        :value_(value), l_(l), r_(r)
    {}
    T value() const { return value_; }
    Binary_tree_node(Binary_tree_node&& o) :value_(o.value_), l_(o.l_), r_(o.r_) {
        o.l_ = nullptr;
        o.r_ = nullptr;
    }
    Binary_tree_node& operator=(Binary_tree_node&& o) {
        value_ = o.value_;
        std::swap(l_, o.l_);
        std::swap(r_, o.r_);
        return *this;
    }
    ~Binary_tree_node() {
        delete l_;
        delete r_;
    }
};

template<typename T>
class Binary_tree_printer_node_handler : public Tree_printer_node_handler<Binary_tree_node<T>> {
    public:
        template<typename F>
            void iterate_node_children(const Binary_tree_node<T>& n, F f) {
                f(static_cast<const Binary_tree_node<T>*>(n.l_));
                f(static_cast<const Binary_tree_node<T>*>(n.r_));
            }
        bool node_is_empty(const Binary_tree_node<T>& n) {
            return !n.l_ && !n.r_;
        }
};

template<typename T>
using Binary_tree_printer = Tree_printer<Binary_tree_node<T>, Binary_tree_printer_node_handler<T>>;

template<typename T>
std::ostream& operator<<(std::ostream& stream, const Binary_tree_node<T>& node) {
    static Binary_tree_printer<T> printer;
    printer.print(node, stream);
    return stream;
}
