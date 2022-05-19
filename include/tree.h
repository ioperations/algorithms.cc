#pragma once

#include <iostream>

#include "array.h"
#include "forward_list.h"
#include "tree_printer.h"

template <typename T, typename C>
class BaseTreeNode {
   private:
    T m_value;
    C m_children;

   public:
    BaseTreeNode(T data) : m_value(data) {}
    BaseTreeNode(T data, C&& children)
        : m_value(data), m_children(std::move(children)) {}
    BaseTreeNode() = default;

    BaseTreeNode(const BaseTreeNode&) = delete;
    BaseTreeNode& operator=(const BaseTreeNode&) = delete;

    BaseTreeNode(BaseTreeNode&& o)
        : m_value(std::move(o.m_value)), m_children(std::move(o.m_children)) {}
    BaseTreeNode& operator=(BaseTreeNode&& o) {
        std::swap(m_value, o.m_value);
        std::swap(m_children, o.m_children);
        return *this;
    }

    T value() const { return m_value; }
    const C& children() const { return m_children; }
    C& children() { return m_children; }
};

template <typename T>
class ArrayTreeNode : public BaseTreeNode<T, Array<ArrayTreeNode<T>>> {
   public:
    using Base = BaseTreeNode<T, Array<ArrayTreeNode<T>>>;
    ArrayTreeNode(T data, Array<ArrayTreeNode>&& children)
        : Base(data, std::move(children)) {}
    ArrayTreeNode(T data) : Base(data) {}
    ArrayTreeNode() = default;
};

template <typename T>
class ForwardListTreeNode
    : public BaseTreeNode<T, ForwardList<ForwardListTreeNode<T>>> {
   public:
    using Base = BaseTreeNode<T, ForwardList<ForwardListTreeNode<T>>>;
    ForwardListTreeNode(T data, ForwardList<ForwardListTreeNode>&& children)
        : Base(data, std::move(children)) {}
    ForwardListTreeNode(T data) : Base(data) {}
    ForwardListTreeNode() = default;
};

template <typename T, typename C>
std::ostream& operator<<(std::ostream& stream, const BaseTreeNode<T, C>& node) {
    TreePrinter<BaseTreeNode<T, C>>::default_instance().print(node, stream);
    return stream;
}
