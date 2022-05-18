#pragma once

#include <iostream>
#include <sstream>

#include "array.h"
#include "forward_list.h"
#include "string_utils.h"

class TreePrinterBase {
   public:
    class Siblings;

    class PrintedNode {
       private:
        friend class Siblings;
        const std::string m_label;
        const int m_label_width;
        const int m_label_half_width;
        int m_position;

        PrintedNode(std::string&& label, size_t width, Siblings* siblings)
            : m_label(label),
              m_label_width(width),
              m_label_half_width(width / 2),
              m_position(0),
              m_next(nullptr),
              m_siblings(siblings) {}
        PrintedNode(PrintedNode&&) = delete;
        PrintedNode* m_next;

       public:
        Siblings* const m_siblings;

        const std::string label() const { return m_label; }

        int position() { return m_position; }
        void position(int p) { m_position = p; }

        int center() const { return m_position + m_label_half_width; }
        void center(int c) { m_position = c - m_label_half_width; }

        void shift(int s) { m_position += s; }
        int border() const { return m_position + m_label_width + 2; }
        size_t label_width() const { return m_label_width; }
        PrintedNode* next() { return m_next; }
    };

    class Siblings {
       private:
        PrintedNode* m_head;
        PrintedNode* m_tail;

       public:
        PrintedNode* const m_parent;
        Siblings* m_next;

        Siblings(PrintedNode* parent)
            : m_head(nullptr),
              m_tail(nullptr),
              m_parent(parent),
              m_next(nullptr) {}
        ~Siblings() {
            for (PrintedNode* node = m_head; node;) {
                auto* previous = node;
                node = node->m_next;
                delete previous;
            }
        }
        Siblings(Siblings&& o) = delete;
        PrintedNode* head() { return m_head; }
        auto add_node(std::string&& label, size_t width) {
            PrintedNode* node = new PrintedNode(std::move(label), width, this);
            if (m_tail)
                m_tail->m_next = node;
            else
                m_head = node;
            m_tail = node;
            return node;
        }
        void fix_positions(Siblings* previous = nullptr);
        template <typename F>
        void for_each(F func) {
            for (auto* node = m_head; node; node = node->m_next) {
                func(node);
            }
        }
    };

    using Line = ForwardList<Siblings>;
    using Lines = ForwardList<Line>;

    void print(Lines& lines, std::ostream& stream);
    ForwardList<std::string> compose_text_lines(Lines& lines);
    template <typename A>
    void print(Lines& lines, A&& appender);
};

template <typename N>
class TreePrinterNodeHandler {
   public:
    std::string node_to_string(const N& node) {
        std::stringstream ss;
        ss << node.value();
        return ss.str();
    }
    size_t label_width(const N& node, const std::string& label) {
        return label.size();
    }
    template <typename F>
    void iterate_node_children(const N& n, F f) {
        auto& children = n.children();
        for (auto child = children.cbegin(); child != children.cend(); ++child)
            f(&*child);
    }
    bool node_is_empty(const N& n) { return false; }
};

template <typename N, typename NH = TreePrinterNodeHandler<N>>
class TreePrinter : protected NH, protected TreePrinterBase {
   private:
    Lines compose_lines(const N& node) {
        struct NodeInfo {
            const N* m_node;
            int m_level;
            PrintedNode* m_parent;
            NodeInfo(const N* node, int level, PrintedNode* parent)
                : m_node(node), m_level(level), m_parent(parent) {}
        };

        ForwardList<NodeInfo> queue;
        queue.emplace_back(&node, 1, nullptr);
        int level = 0;
        Lines lines;

        while (!queue.empty()) {
            auto n = queue.pop_front();
            if (n.m_level > level) {
                lines.emplace_back();
                level = n.m_level;
            }
            auto& line = lines.back();
            if (line.empty()) line.emplace_back(n.m_parent);
            Siblings* siblings = &line.back();
            if (siblings->m_parent != n.m_parent) {
                line.emplace_back(n.m_parent);
                Siblings* previous = siblings;
                siblings = &line.back();
                previous->m_next = siblings;
            }
            if (n.m_node) {
                auto label = NH::node_to_string(n.m_node->value());
                auto label_width = NH::label_width(*n.m_node, label);
                auto printable_node =
                    siblings->add_node(std::move(label), label_width);
                if (!NH::node_is_empty(*n.m_node))
                    NH::iterate_node_children(
                        *n.m_node, [&queue, n, printable_node](auto child) {
                            queue.emplace_back(child, n.m_level + 1,
                                               printable_node);
                        });
            } else
                siblings->add_node(
                    " ",
                    1);  // todo add custom empty node rendering to stringifier
        }
        return lines;
    }

   public:
    void print(const N& node, std::ostream& stream) {
        auto lines = compose_lines(node);
        TreePrinterBase::print(lines, stream);
    }

    ForwardList<std::string> compose_text_lines(const N& node) {
        auto lines = compose_lines(node);
        return TreePrinterBase::compose_text_lines(lines);
    }

    static TreePrinter default_instance() {
        static TreePrinter printer;
        return printer;
    }
};
