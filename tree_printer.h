#pragma once

#include <iostream>
#include <sstream>

#include "array.h"
#include "forward_list.h"

class Tree_printer {
    private:
        struct Siblings;

        class Printed_node {
            private:
                const std::string label_;
                const int label_half_width_;
                int position_;
                Siblings* const siblings_;
            public:
                Printed_node(std::string label, Siblings* siblings)
                    :label_(label), label_half_width_(label.size() / 2), position_(0), siblings_(siblings)
                {}

                const std::string label() { return label_; }

                int position() { return position_; }
                void position(int p) { position_ = p; }

                int center() const { return position_ + label_half_width_; }
                void center(int c) { position_ = c - label_half_width_; }

                void shift(int s) { position_ += s; }
                int border() const { return position_ + label_.size() + 2; }
                void fix_positions(Siblings* previous = nullptr);
        };

        class Siblings : public Forward_list<Printed_node> {
            private:
                Printed_node* const parent_;
                iterator parent_it_;
            public:
                Siblings(iterator parent_it = iterator(nullptr)) 
                    :parent_(parent_it.empty() ? nullptr : &*parent_it), parent_it_(parent_it) 
                    {}
                void fix_positions(Siblings* previous);
                Printed_node* parent() { return parent_; }
        };

        using Line = Forward_list<Siblings>;
        using Lines = Array<Line>;

        template<typename N>
            void populate_lines(const N& node, Lines& lines, Siblings& siblings, int level = 0) {
                std::stringstream ss;
                ss << node.value();
                siblings.emplace_back(ss.str(), &siblings);
                auto parent_it = siblings.before_end();
                ++level;

                auto& children = node.children();
                auto it = children.cbegin();
                if (it != children.cend()) {
                    auto& line = lines[level];
                    line.push_back(Siblings(parent_it));
                    for (; it != children.cend(); ++it)
                        populate_lines(*it, lines, line.back(), level);
                }
            }

        template<typename N>
            int calculate_depth(const N& node, int level = 0) {
                ++level;
                int depth = level;
                auto& children = node.children();
                for (auto it = children.cbegin(); it != children.cend(); ++it)
                    depth = std::max(depth, calculate_depth(*it, level));
                return depth;
            }

        template<typename N>
            Lines compose_lines(const N& node) {
                Lines lines(calculate_depth(node));
                auto& first_line = lines[0];
                first_line.push_back(Siblings());
                populate_lines(node, lines, first_line.back());
                return lines;
            }

        template<typename A>
            void print(Lines& lines, A&& appender);
        void print(Lines& lines, std::ostream& stream);
        Forward_list<std::string> compose_text_lines(Lines& lines); // todo const lines
    public:
        template<typename N>
            void print(const N& node, std::ostream& stream) {
                Lines lines = compose_lines(node);
                print(lines, stream);
            }

        template<typename N>
            Forward_list<std::string> compose_text_lines(const N& node) {
                Lines lines = compose_lines(node);
                return compose_text_lines(lines);
            }

        static Tree_printer default_instance() {
            static Tree_printer printer;
            return printer;
        }
};

