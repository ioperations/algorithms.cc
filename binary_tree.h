#pragma once

#include <iostream>

#include "tree_printer.h"

template <typename T>
struct Binary_tree_node {
    T m_value;
    Binary_tree_node* m_l;
    Binary_tree_node* m_r;
    Binary_tree_node(T value, Binary_tree_node* l = nullptr,
                     Binary_tree_node* r = nullptr)
        : m_value(value), m_l(l), m_r(r) {}
    T value() const { return m_value; }
    Binary_tree_node(Binary_tree_node&& o)
        : m_value(o.m_value), m_l(o.m_l), m_r(o.m_r) {
        o.m_l = nullptr;
        o.m_r = nullptr;
    }
    Binary_tree_node& operator=(Binary_tree_node&& o) {
        m_value = o.m_value;
        std::swap(m_l, o.m_l);
        std::swap(m_r, o.m_r);
        return *this;
    }
    ~Binary_tree_node() {
        delete m_l;
        delete m_r;
    }
};

template <typename T>
class Binary_tree_printer_node_handler
    : public Tree_printer_node_handler<Binary_tree_node<T>> {
   public:
    template <typename F>
    void iterate_node_children(const Binary_tree_node<T>& n, F f) {
        f(static_cast<const Binary_tree_node<T>*>(n.m_l));
        f(static_cast<const Binary_tree_node<T>*>(n.m_r));
    }
    bool node_is_empty(const Binary_tree_node<T>& n) {
        return !n.m_l && !n.m_r;
    }
};

template <typename T>
using Binary_tree_printer =
    Tree_printer<Binary_tree_node<T>, Binary_tree_printer_node_handler<T>>;

template <typename T>
std::ostream& operator<<(std::ostream& stream,
                         const Binary_tree_node<T>& node) {
    static Binary_tree_printer<T> printer;
    printer.print(node, stream);
    return stream;
}
