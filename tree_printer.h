#pragma once

#include <iostream>
#include <sstream>

#include "array.h"
#include "list.h"

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
            void fix_positions(Siblings* previous = nullptr, bool fix_min_positions = false);
        };

        using Line = List<Siblings>;
        using Lines = Array<Line>;

        template<typename N>
            void populate_lines(const N& node, Lines& lines, Siblings& siblings, int level = 0) {
                std::stringstream ss;
                ss << node.value();
                siblings.push_back(Printed_node(ss.str(), &siblings)); // todo add emplace_back method
                auto parent = &siblings.back();
                ++level;

                if (node.children().size() > 0) {
                    auto& line = lines[level];
                    line.push_back(Siblings(parent));
                    auto& children = node.children();
                    for (auto it = children.cbegin(); it != children.cend(); ++it)
                        populate_lines(*it, lines, line.back(), level);
                }
            };

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
                first_line.push_back(Siblings(nullptr));
                populate_lines(node, lines, first_line.back());
                return lines;
            }

        void do_print(Lines& lines, std::ostream& stream);
    public:
        template<typename N>
            void print(const N& node, std::ostream& stream) {
                Lines lines = compose_lines(node);
                do_print(lines, stream);
            }

        static Tree_printer default_instance() {
            static Tree_printer printer;
            return printer;
        }
};
