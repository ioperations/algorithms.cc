#include "tree_printer.h"

void Tree_printer::Siblings::fix_positions(Siblings* previous, bool fix_min_positions) {
    auto middle = [this]() {
        return front().position_ + (back().position_ - front().position_ + 1) / 2;
    };

    auto shift = 0;
    if (previous) {
        auto& previous_node = previous->back();
        shift = previous_node.position_ + previous_node.label_.size() + 2 - front().position_;
    }
    int current_middle = middle();
    if (parent_) 
        shift = std::max(shift, parent_->position_ - current_middle);

    bool shifted = false;
    if (shift > 0)
        shifted = true;

    if (shift > 0 || fix_min_positions) { // todo pass parent as iterator and fix min pos for nodes after parent only?
        Printed_node* previous_node = nullptr;
        for (auto& node : *this) {
            node.position_ += shift;
            if (previous_node) {
                int min_position = previous_node->position_ + previous_node->label_.size() + 2;
                if (min_position > node.position_) {
                    node.position_ = min_position;
                    shifted = true;
                }
            }
            previous_node = &node;
        }
    }

    if (parent_) {
        if (shifted)
            current_middle = middle();
        if (parent_->position_ != current_middle) {
            parent_->position_ = current_middle;
            parent_->siblings_->fix_positions(nullptr, true);
        }
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

    for (int i = 1; i < lines.size(); ++i) {
        Siblings* previous = nullptr;
        for (auto& siblings : lines[i]) {
            siblings.fix_positions(previous);
            previous = &siblings;
        }
    }

    struct Appender {
        std::ostream& stream_;
        int count_;
        Appender(std::ostream& stream) :stream_(stream), count_(0) {}
        void operator<<(const char* s) {
            stream_ << s;
            ++count_;
        }
        void operator<<(const std::string& s) {
            stream_ << s;
            count_ += s.size();
        }
        void repeat_until(int bound, const char* s) {
            while (count_ < bound)
                *this << s;
        }
    };

    for (auto line = lines.begin(); line != lines.end(); ++line) {
        if (line != lines.begin()) {
            stream << std::endl;
            Appender appender(stream);
            for (auto& siblings : *line) {
                auto node = siblings.begin();

                bool more_than_one = false;
                if (node != siblings.end()) {
                    auto second = node;
                    ++second;
                    more_than_one = second != siblings.end();
                }
                auto parent = siblings.parent_;
                auto parent_position = parent->position_ + parent->label_.size() / 2;

                if (more_than_one) {
                    appender.repeat_until(node->position_ + node->label_.size() / 2, " ");
                    ++node;
                    bool first = true;
                    for (; node != siblings.end(); ++node) {
                        if (first) {
                            appender << "┌";
                            first = false;
                        } else if (parent_position == appender.count_)
                            appender << "┼";
                        else
                            appender << "┬";
                        auto end = node->position_ + (node->label_.size()) / 2;
                        if (parent_position > appender.count_ && parent_position < end) {
                            appender.repeat_until(parent_position, "─");
                            appender << "┴";
                            appender.repeat_until(end, "─");
                        } else
                            appender.repeat_until(end, "─");
                    }
                    appender << "┐";
                } else {
                    appender.repeat_until(parent_position, " ");
                    appender << "│";
                }
            }
            stream << std::endl;
        }
        Appender appender(stream);
        for (auto& siblings : *line)
            for (auto& node : siblings) {
                appender.repeat_until(node.position_, " ");
                appender << node.label_;
            }
    }
}
