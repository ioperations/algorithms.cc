#pragma once

#include <iostream>

#include "tree_printer.h"

template <typename T>
struct BinaryTreeNode {
    T m_value;
    BinaryTreeNode* m_l;
    BinaryTreeNode* m_r;
    BinaryTreeNode(T value, BinaryTreeNode* l = nullptr,
                   BinaryTreeNode* r = nullptr)
        : m_value(value), m_l(l), m_r(r) {}
    T value() const { return m_value; }
    BinaryTreeNode(BinaryTreeNode&& o)
        : m_value(o.m_value), m_l(o.m_l), m_r(o.m_r) {
        o.m_l = nullptr;
        o.m_r = nullptr;
    }
    BinaryTreeNode& operator=(BinaryTreeNode&& o) {
        m_value = o.m_value;
        std::swap(m_l, o.m_l);
        std::swap(m_r, o.m_r);
        return *this;
    }
    ~BinaryTreeNode() {
        delete m_l;
        delete m_r;
    }
};

template <typename T>
class BinaryTreePrinterNodeHandler
    : public TreePrinterNodeHandler<BinaryTreeNode<T>> {
   public:
    template <typename F>
    void iterate_node_children(const BinaryTreeNode<T>& n, F f) {
        f(static_cast<const BinaryTreeNode<T>*>(n.m_l));
        f(static_cast<const BinaryTreeNode<T>*>(n.m_r));
    }
    bool node_is_empty(const BinaryTreeNode<T>& n) { return !n.m_l && !n.m_r; }
};

template <typename T>
using Binary_tree_printer =
    TreePrinter<BinaryTreeNode<T>, BinaryTreePrinterNodeHandler<T>>;

template <typename T>
std::ostream& operator<<(std::ostream& stream, const BinaryTreeNode<T>& node) {
    static Binary_tree_printer<T> printer;
    printer.print(node, stream);
    return stream;
}
