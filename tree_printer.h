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
                const std::string label_;
                const int label_width_;
                const int label_half_width_;
                int position_;
                Siblings* const siblings_;
            public:
                Printed_node(std::string label, size_t width, Siblings* siblings)
                    :label_(label), label_width_(width), label_half_width_(width / 2), position_(0), siblings_(siblings)
                {}

                const std::string label() const { return label_; }

                int position() { return position_; }
                void position(int p) { position_ = p; }

                int center() const { return position_ + label_half_width_; }
                void center(int c) { position_ = c - label_half_width_; }

                void shift(int s) { position_ += s; }
                int border() const { return position_ + label_width_ + 2; }
                void fix_positions(Siblings* previous = nullptr);
                size_t label_width() const { return label_width_; }
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

        template<typename A>
            void print(Lines& lines, A&& appender);
        void print(Lines& lines, std::ostream& stream);
        Forward_list<std::string> compose_text_lines(Lines& lines); // todo const lines
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
    size_t operator()(const N& node, const std::string label) {
        return string_length(label);
    }
};

template<typename N, typename S = Default_stringifier<N>, typename L = Default_label_width_calculator<N>>
class Tree_printer : protected Tree_printer_base {
    private:
        S stringifier_;
        L label_width_calculator_;
        void populate_lines(const N& node, Lines& lines, Siblings& siblings, int level = 0) {
            auto string = stringifier_(node.value());
            siblings.emplace_back(string, label_width_calculator_(node, string), &siblings);
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

        int calculate_depth(const N& node, int level = 0) {
            ++level;
            int depth = level;
            auto& children = node.children();
            for (auto it = children.cbegin(); it != children.cend(); ++it)
                depth = std::max(depth, calculate_depth(*it, level));
            return depth;
        }

        Lines compose_lines(const N& node) {
            Lines lines(calculate_depth(node));
            auto& first_line = lines[0];
            first_line.push_back(Siblings());
            populate_lines(node, lines, first_line.back());
            return lines;
        }
    public:
        void print(const N& node, std::ostream& stream) {
            Lines lines = compose_lines(node);
            Tree_printer_base::print(lines, stream);
        }

        Forward_list<std::string> compose_text_lines(const N& node) {
            Lines lines = compose_lines(node);
            return Tree_printer_base::compose_text_lines(lines);
        }

        static Tree_printer default_instance() {
            static Tree_printer printer;
            return printer;
        }
};

