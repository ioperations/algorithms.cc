#include "tree_printer.h"

#include "box_drawing_chars.h"

void TreePrinterBase::Siblings::fix_positions(Siblings* previous) {
    auto middle = [this]() {
        int middle = m_head->center();
        if (m_head != m_tail)
            middle = middle + (m_tail->center() - middle + 1) / 2;
        return middle;
    };

    auto shift = 0;
    if (previous) shift = previous->m_tail->border() - m_head->position();
    int current_middle = middle();
    if (m_parent) shift = std::max(shift, m_parent->center() - current_middle);

    if (shift > 0)
        for (auto* node = m_head; node; node = node->next()) node->shift(shift);

    if (m_parent) {
        if (shift > 0) current_middle = middle();
        if (m_parent->center() != current_middle) {
            m_parent->center(current_middle);

            auto* previous = m_parent;
            for (auto* sibling = m_parent->next(); sibling;
                 sibling = sibling->next()) {
                int min_position = previous->border();
                if (min_position > sibling->position())
                    sibling->position(min_position);
                previous = sibling;
            }

            m_parent->m_siblings->fix_positions();

            auto* previous_siblings = m_parent->m_siblings;
            for (auto* siblings = previous_siblings->m_next; siblings;
                 siblings = siblings->m_next) {
                siblings->fix_positions(previous_siblings);
                previous_siblings = siblings;
            }
        }
    }
}

template <typename S>
class BaseAppender {
   protected:
    S m_stream;
    int m_count;
    BaseAppender(S&& stream) : m_stream(std::forward<S>(stream)), m_count(0) {}

   public:
    void operator<<(const char* s) {
        m_stream << s;
        ++m_count;
    }
    void operator<<(const std::string& s) {
        m_stream << s;
        m_count += s.size();
    }
    void operator<<(const TreePrinterBase::PrintedNode* node) {
        m_stream << node->label();
        m_count += node->label_width();
    }
    void repeat_until(int bound, const char* s) {
        while (m_count < bound) *this << s;
    }
    int count() { return m_count; }
    void reset() { m_count = 0; }
};

void TreePrinterBase::print(Lines& lines, std::ostream& stream) {
    struct Appender : BaseAppender<std::ostream&> {
        Appender(std::ostream& stream) : BaseAppender<std::ostream&>(stream) {}
        void new_line() { m_stream << std::endl; }
    };
    print(lines, Appender(stream));
}

ForwardList<std::string> TreePrinterBase::compose_text_lines(Lines& lines) {
    struct Appender : BaseAppender<std::stringstream> {
        ForwardList<std::string> m_lines;
        Appender() : BaseAppender<std::stringstream>(std::stringstream()) {}
        void new_line() {
            m_lines.push_back(m_stream.str());
            m_stream = std::stringstream();
        }
    };
    Appender appender;
    print(lines, appender);
    appender.new_line();
    return std::move(appender.m_lines);
}

template <typename A>
void TreePrinterBase::print(Lines& lines, A&& appender) {
    for (auto& line : lines) {
        PrintedNode* previous = nullptr;
        for (auto& siblings : line) {
            siblings.for_each([&previous](auto node) {
                if (previous) node->position(previous->border());
                previous = node;
            });
        }
    }

    namespace bc = Box_drawing_chars;

    auto do_print = [&lines, &appender]() {
        for (auto line = lines.begin(); line != lines.end(); ++line) {
            if (line != lines.begin()) {
                appender.new_line();
                appender.reset();
                for (auto& siblings : *line) {
                    auto* node = siblings.head();

                    auto parent_center = siblings.m_parent->center();

                    if (node && node->next()) {
                        appender.repeat_until(node->center(), " ");
                        node = node->next();
                        bool first = true;
                        for (; node; node = node->next()) {
                            if (first) {
                                appender << bc::right_bottom;
                                first = false;
                            } else if (parent_center == appender.count())
                                appender << bc::cross;
                            else
                                appender << bc::bottom_t;
                            auto end = node->center();
                            if (parent_center > appender.count() &&
                                parent_center < end) {
                                appender.repeat_until(parent_center,
                                                      bc::h_line);
                                appender << bc::top_t;
                                appender.repeat_until(end, bc::h_line);
                            } else
                                appender.repeat_until(end, bc::h_line);
                        }
                        appender << bc::left_bottom;
                    } else {
                        appender.repeat_until(parent_center, " ");
                        appender << bc::v_line;
                    }
                }
                appender.new_line();
            }
            appender.reset();
            for (auto& siblings : *line)
                siblings.for_each([&appender](auto node) {
                    appender.repeat_until(node->position(), " ");
                    appender << node;
                });
        }
    };

    auto line = lines.begin();
    ++line;
    for (; line != lines.end(); ++line) {
        Siblings* previous = nullptr;
        for (auto& siblings : *line) {
            siblings.fix_positions(previous);
            previous = &siblings;
        }
    }

    do_print();
}
