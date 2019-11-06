#pragma once

#include <iostream>
#include <sstream>

#include "array.h"
#include "forward_list.h"
#include "string_utils.h"

class Tree_printer_base {
    protected:
        struct Siblings;

        class Printed_node {
            private:
                friend class Siblings;
                const std::string label_;
                const int label_width_;
                const int label_half_width_;
                int position_;

                Printed_node(std::string&& label, size_t width, Siblings* siblings)
                    :label_(label), label_width_(width), label_half_width_(width / 2), position_(0), next_(nullptr), siblings_(siblings)
                {}
                Printed_node(Printed_node&&) = delete;
                Printed_node* next_;
            public:
                Siblings* const siblings_;

                const std::string label() const { return label_; }

                int position() { return position_; }
                void position(int p) { position_ = p; }

                int center() const { return position_ + label_half_width_; }
                void center(int c) { position_ = c - label_half_width_; }

                void shift(int s) { position_ += s; }
                int border() const { return position_ + label_width_ + 2; }
                size_t label_width() const { return label_width_; }
                Printed_node* next() { return next_; }
        };

        class Siblings {
            private:
                Printed_node* head_;
                Printed_node* tail_;
            public:
                Printed_node* const parent_;
                Siblings* next_;

                Siblings(Printed_node* parent) 
                    :head_(nullptr), tail_(nullptr), parent_(parent), next_(nullptr)
                {}
                ~Siblings() {
                    for (Printed_node* node = head_; node; ) {
                        auto previous = node;
                        node = node->next_;
                        delete previous;
                    }
                }
                Siblings(Siblings&& o) = delete;
                Printed_node* head() { return head_; }
                auto add_node(std::string&& label, size_t width) {
                    Printed_node* node = new Printed_node(std::move(label), width, this);
                    if (tail_) tail_->next_ = node;
                    else head_ = node;
                    tail_ = node;
                    return node;
                }
                void fix_positions(Siblings* previous = nullptr);
                template<typename F>
                    void for_each(F func) {
                        for (auto node = head_; node; node = node->next_) {
                            func(node);
                        }
                    }
        };

        using Line = Forward_list<Siblings>;
        using Lines = Array<Line>;

        void print(Lines& lines, std::ostream& stream);
        Forward_list<std::string> compose_text_lines(Lines& lines);
        template<typename A>
            void print(Lines& lines, A&& appender);
};

template<typename N>
struct Default_stringifier {
    std::string operator()(const N& node) {
        std::stringstream ss;
        ss << node.value();
        return ss.str();
    }
};

template<typename N>
struct Default_label_width_calculator {
    size_t operator()(const N& node, const std::string& label) {
        return label.size();
    }
};

template<typename N, typename S = Default_stringifier<N>, typename L = Default_label_width_calculator<N>>
class Tree_printer : protected Tree_printer_base {
    private:
        S stringifier_;
        L label_width_calculator_;

        Lines compose_lines(const N& node) {
            auto depth = calculate_depth(node);
            Lines lines(depth);
            populate_lines(node, lines);
            return lines;
        }

        int calculate_depth(const N& node, int level = 0) {
            ++level;
            int depth = level;
            auto& children = node.children();
            for (auto child = children.cbegin(); child != children.cend(); ++child)
                depth = std::max(depth, calculate_depth(*child, level));
            return depth;
        }

        void populate_lines(const N& node, Lines& lines, Printed_node* parent = nullptr, int level = 0) {
            auto& line = lines[level];
            if (line.empty())
                line.emplace_back(parent);
            Siblings* siblings = &line.back();
            if (siblings->parent_ != parent) {
                line.emplace_back(parent);
                Siblings* previous = siblings;
                siblings = &line.back();
                previous->next_ = siblings;
            }
            auto label = stringifier_(node.value());
            auto label_width = label_width_calculator_(node, label);
            auto printable_node = siblings->add_node(std::move(label), label_width);

            auto child = node.children().cbegin();
            if (child != node.children().cend()) {
                ++level;
                do {
                    populate_lines(*child, lines, printable_node, level);
                    ++child;
                } while (child != node.children().cend());
            }
        }

    public:
        void print(const N& node, std::ostream& stream) {
            auto lines = compose_lines(node);
            Tree_printer_base::print(lines, stream);
        }

        Forward_list<std::string> compose_text_lines(const N& node) {
            auto lines = compose_lines(node);
            return Tree_printer_base::compose_text_lines(lines);
        }

        static Tree_printer default_instance() {
            static Tree_printer printer;
            return printer;
        }
};

