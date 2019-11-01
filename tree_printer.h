#pragma once

#include "tree.h"

#include <iostream>

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
            void fix_positions();
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

        void print(Lines& lines, std::ostream& stream);
    public:
        template<typename T>
            void print(Node<T>& node, std::ostream& stream) {
                Lines lines = compose_lines(node);
                print(lines, stream);
            }
};
