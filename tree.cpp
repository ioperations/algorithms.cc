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

struct Siblings;

struct Printed_node {
    std::string label_;
    int position_;
    Siblings* const siblings_;
    Printed_node(std::string label, Siblings* siblings)
        :label_(label), position_(0), siblings_(siblings)
    {}
};

std::ostream& operator<<(std::ostream& stream, const Printed_node& node) {
    stream << node.label_ << "[" << node.position_ << "]";
    return stream;
}

struct Siblings : public List<Printed_node> {
    Printed_node* parent_;
    Siblings(Printed_node* parent) :parent_(parent) {}
    void foo() {
        auto middle = [this]() {
            return front().position_ + (back().position_ - front().position_ + 1) / 2;
        };
        if (parent_) {
            int shift = parent_->position_ - middle();
            if (shift > 0)
                for (auto& node : *this)
                    node.position_ += shift; // todo check previous siblings group position and shift further if required
            
            parent_->position_ = middle();
            parent_->siblings_->foo();
        }
    }
};

using Line = List<Siblings>;
using Lines = Array<Line>;

void populate_lines(node& node, Lines& lines, Siblings& siblings, int level = 0) {
    siblings.push_back(Printed_node(std::to_string(node.value()), &siblings)); // todo add emplace_back method
    auto parent = &siblings.back();
    ++level;

    if (node.children().size() > 0) {
        auto& line = lines[level];
        line.push_back(Siblings(parent));
        for (auto& child : node.children())
            populate_lines(child, lines, line.back(), level);
    }
};

Lines compose_lines(node& node) {
    Lines lines(node.depth());
    auto& first_line = lines[0];
    first_line.push_back(Siblings(nullptr));
    populate_lines(node, lines, first_line.back());
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

    for (auto& line : lines) {
        Printed_node* previous = nullptr;
        for (auto& siblings : line) {
            for (auto& node : siblings) {
                if (previous)
                    node.position_ = previous->position_ + previous->label_.size() + 2;
                previous = &node;
            }
        }
    }

    auto print = [&lines]() {
        auto middle = [](int i) { return (i + 1) / 2; };
        for (auto it = lines.begin(); it != lines.end(); ++it) {
            auto& line = *it;
            if (it != lines.begin()) {
                int chars_count = 0;
                for (auto& siblings : line) {

                    auto node_it = siblings.begin();
                    {
                        auto& node = *node_it;
                        for (; chars_count < node.position_ + node.label_.size() / 2; ++chars_count)
                            std::cout << " ";
                    }
                    if (node_it != siblings.end()) ++node_it;
                    bool first = true;
                    for (; node_it != siblings.end(); ++node_it) {
                        auto& node = *node_it;
                        if (first) {
                            std::cout << "┌";
                            first = false;
                        } else
                            std::cout << "┬";
                        ++chars_count;
                        for (; chars_count < node.position_ + (node.label_.size() + 1) / 2; ++chars_count)
                            std::cout << "─";
                    }
                    std::cout << "┐";
                    ++chars_count;
                }
                std::cout << std::endl;
            }
            int chars_count = 0;
            for (auto& siblings : line)
                for (auto& node : siblings) {
                    for (int i = 0; i < node.position_ - chars_count; ++i) 
                        std::cout << " ";
                    std::cout << node.label_;
                    chars_count = node.position_ + node.label_.size();
                }
            std::cout << std::endl;
        }
    };

    print();

    for (int i = 1; i < lines.size(); ++i)
        for (auto& siblings : lines[i])
            siblings.foo();

    print();
}
