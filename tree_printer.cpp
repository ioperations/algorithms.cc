#include "tree_printer.h"

void Tree_printer::Printed_node::fix_positions(Siblings* previous) {
    siblings_->fix_positions(previous);
}

void Tree_printer::Siblings::fix_positions(Siblings* previous) {
    auto middle = [this]() {
        int b = front().center();
        return b + (back().center() - b + 1) / 2;
    };

    auto shift = 0;
    if (previous)
        shift = previous->back().border() - front().position();
    int current_middle = middle();
    if (parent_) 
        shift = std::max(shift, parent_->center() - current_middle);

    if (shift > 0)
        for (auto& node : *this)
            node.shift(shift);

    if (parent_) {
        if (shift > 0)
            current_middle = middle();
        if (parent_->center() != current_middle) {
            parent_->center(current_middle);
            auto previous_node = parent_;
            auto parent_sibling = parent_it_;
            ++parent_sibling;
            for (; !parent_sibling.empty(); ++parent_sibling) {
                int min_position = previous_node->border();
                if (min_position > parent_sibling->position())
                    parent_sibling->position(min_position);
                previous_node = &*parent_sibling;
            }
            parent_->fix_positions();
        }
    }
}

template<typename S>
class Base_appender {
    protected:
        S stream_;
        int count_;
        Base_appender(S&& stream): stream_(std::forward<S>(stream)), count_(0) {}
    public:
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
        int count() { return count_; }
        void reset() { count_ = 0; }
};

void Tree_printer::print(Lines& lines, std::ostream& stream) {
    struct Appender : Base_appender<std::ostream&> {
        Appender(std::ostream& stream) :Base_appender<std::ostream&>(stream) {}
        void new_line() {
            stream_ << std::endl;
        }
    };
    print(lines, Appender(stream));
}

Forward_list<std::string> Tree_printer::compose_text_lines(Lines& lines) {
    struct Appender : Base_appender<std::stringstream> {
        Forward_list<std::string> lines_;
        Appender(): Base_appender<std::stringstream>(std::stringstream()) {}
        void new_line() {
            lines_.push_back(stream_.str());
            stream_ = {};
        }
    };
    Appender appender;
    print(lines, appender);
    appender.new_line();
    return std::move(appender.lines_);
}

template<typename A>
void Tree_printer::print(Lines& lines, A&& appender) {
    for (auto& line : lines) {
        Printed_node* previous = nullptr;
        for (auto& siblings : line) {
            for (auto& node : siblings) {
                if (previous)
                    node.position(previous->border());
                previous = &node;
            }
        }
    }

    for (size_t i = 1; i < lines.size(); ++i) {
        Siblings* previous = nullptr;
        for (auto& siblings : lines[i]) {
            siblings.fix_positions(previous);
            previous = &siblings;
        }
    }

    for (auto line = lines.begin(); line != lines.end(); ++line) {
        if (line != lines.begin()) {
            appender.new_line();
            appender.reset();
            for (auto& siblings : *line) {
                auto node = siblings.begin();

                bool more_than_one = false;
                if (node != siblings.end()) {
                    auto second = node;
                    ++second;
                    more_than_one = second != siblings.end();
                }
                auto parent_center = siblings.parent()->center();

                if (more_than_one) {
                    appender.repeat_until(node->center(), " ");
                    ++node;
                    bool first = true;
                    for (; node != siblings.end(); ++node) {
                        if (first) {
                            appender << "┌";
                            first = false;
                        } else if (parent_center == appender.count())
                            appender << "┼";
                        else
                            appender << "┬";
                        auto end = node->center();
                        if (parent_center > appender.count() && parent_center < end) {
                            appender.repeat_until(parent_center, "─");
                            appender << "┴";
                            appender.repeat_until(end, "─");
                        } else
                            appender.repeat_until(end, "─");
                    }
                    appender << "┐";
                } else {
                    appender.repeat_until(parent_center, " ");
                    appender << "│";
                }
            }
            appender.new_line();
        }
        appender.reset();
        for (auto& siblings : *line)
            for (auto& node : siblings) {
                appender.repeat_until(node.position(), " ");
                appender << node.label();
            }
    }
}
