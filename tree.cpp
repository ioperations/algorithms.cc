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

class Tree_printer {
    private:
        struct Siblings;

        struct Printed_node {
            std::string label_;
            int position_;
            Siblings* const siblings_;
            Printed_node(std::string label, Siblings* siblings)
                :label_(label), position_(0), siblings_(siblings)
            {}
        };

        struct Siblings : public List<Printed_node> {
            Printed_node* parent_;
            Siblings(Printed_node* parent) :parent_(parent) {}
            void fix_positions() {
                auto middle = [this]() {
                    return front().position_ + (back().position_ - front().position_ + 1) / 2;
                };
                if (parent_) {
                    int shift = parent_->position_ - middle();
                    if (shift > 0)
                        for (auto& node : *this)
                            node.position_ += shift; // todo check previous siblings group position and shift further if required

                    parent_->position_ = middle();
                    parent_->siblings_->fix_positions();
                }
            }
        };

        using Line = List<Siblings>;
        using Lines = Array<Line>;

        template<typename T>
            void populate_lines(Node<T>& node, Lines& lines, Siblings& siblings, int level = 0) {
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

        template<typename T>
            Lines compose_lines(Node<T>& node) {
                Lines lines(node.depth());
                auto& first_line = lines[0];
                first_line.push_back(Siblings(nullptr));
                populate_lines(node, lines, first_line.back());
                return lines;
            }

    public:
        void print(Lines& lines, std::ostream& stream) {
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

                for (int i = 1; i < lines.size(); ++i)
                    for (auto& siblings : lines[i])
                        siblings.fix_positions();

                struct Appender {
                    std::ostream& stream_;
                    int count_;
                    Appender(std::ostream& stream) :stream_(stream), count_(0) {}
                    void operator<<(const char* s) {
                        stream_ << s;
                        ++count_;
                    }
                    void repeat(int bound, const char* s) {
                        while (count_ < bound)
                            *this << s;
                    }
                };

                for (auto line = lines.begin(); line != lines.end(); ++line) {
                    if (line != lines.begin()) {
                        Appender appender(stream);
                        for (auto& siblings : *line) {
                            auto node = siblings.begin();
                            appender.repeat(node->position_ + node->label_.size() / 2, " ");
                            if (node != siblings.end())
                                ++node;
                            bool first = true;
                            auto parent = siblings.parent_;
                            auto parent_position = parent->position_ + parent->label_.size() / 2;
                            for (; node != siblings.end(); ++node) {
                                if (first) {
                                    appender << "┌";
                                    first = false;
                                } else
                                    appender << "┬";
                                auto end = node->position_ + (node->label_.size() + 1) / 2;
                                if (parent_position > appender.count_ && parent_position < end) {
                                    appender.repeat(parent_position, "─");
                                    appender << "┴";
                                    appender.repeat(end, "─");
                                } else
                                    appender.repeat(end, "─");
                            }
                            appender << "┐";
                        }
                        stream << std::endl;
                    }
                    int chars_count = 0;
                    for (auto& siblings : *line)
                        for (auto& node : siblings) {
                            for (int i = 0; i < node.position_ - chars_count; ++i) 
                                stream << " ";
                            stream << node.label_;
                            chars_count = node.position_ + node.label_.size();
                        }
                    stream << std::endl;
                }
        }
        template<typename T>
            void print(Node<T>& node, std::ostream& stream) {
                Lines lines = compose_lines(node);
                print(lines, stream);
            }
};

int main() {
    using node = Node<int>;
    using nodes = Array<node>;
    node n(1, nodes::build_array(
            node(11, nodes::build_array(111, 112, 113)),
            node(12, nodes::build_array(121, 122)),
            node(13, nodes::build_array(
                    node(131, nodes::build_array(1311, 1312, 1313)),
                    132, 133))
            ));
    Tree_printer().print(n, std::cout);
}
