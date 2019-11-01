#include "tree_printer.h"

void Tree_printer::Siblings::fix_positions() {
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

void Tree_printer::do_print(Lines& lines, std::ostream& stream) {
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
