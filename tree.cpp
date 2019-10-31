#include <cstddef>
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

using node = Node<int>;

struct Printed_node {
    std::string label_;
    int offset_;
    Printed_node* parent_;
    Printed_node(std::string label, int offset, Printed_node* parent)
        :label_(label), offset_(offset), parent_(parent)
    {}
};

using Line = List<Printed_node>;
using Lines = Array<Line>;

void populate_lines(node& node, Lines& lines, Printed_node* parent = nullptr, int level = 0) {
    auto& line = lines[level];
    int offset = line.empty() ? 0 : 2;
    line.push_back(Printed_node(std::to_string(node.value()), offset, parent)); // todo add emplace_back method
    parent = &line.back();
    ++level;
    for (auto& child : node.children())
        populate_lines(child, lines, parent, level);
};

Lines compose_lines(node& node) {
    Lines lines(node.depth());
    populate_lines(node, lines);
    return lines;
}

int main() {
    using nodes = Array<node>;
    node n(1, nodes::build_array(
            node(11, nodes::build_array(111, 112, 113)),
            node(12, nodes::build_array(121, 122)),
            node(13, nodes::build_array(
                    node(131, nodes::build_array(1311, 1312, 1313)),
                    132, 133))
            ));

    Lines lines = compose_lines(n);

    auto print = [&lines]() {
        for (auto& line : lines) {
            for (auto& node : line) {
                for (int i = 0; i < node.offset_; ++i) std::cout << " ";
                std::cout << node.label_;
            }
            std::cout << std::endl;
        }
    };

    print();

    for (int i = 1; i < lines.size(); ++i) {
        Line& line = lines[i];
        Printed_node* previous = nullptr;
        int previous_length = 0;
        int length = 0;
        auto foo = [&]() {
            // std::cout << "node: " << previous->label_ << std::endl;
            // std::cout << "parent: " << previous->parent_->label_ << std::endl;
            // std::cout << "length: " << length << std::endl;

            previous->parent_->offset_ = previous_length + length / 2;
        };
        for (auto& current : line) {
            bool first = previous && previous->parent_ != current.parent_;
            if (first) {
                foo();
                previous_length += length / 2;
                length = 0;
            }
            length += current.label_.size();
            if (first)
                previous += current.offset_;
            else
                length += current.offset_;
            previous = &current;
        }
        if (previous) {
            foo();
        }
    }

    print();

}
