#pragma once
#include "forward_list.h"
#include "string_utils.h"

class TextBlock {
   public:
    using Lines = ForwardList<std::string>;

   private:
    Lines m_lines;
    size_t m_width = 0;
    int m_lines_count = 0;

   public:
    TextBlock(Lines&& lines) : m_lines(std::move(lines)) {
        for (auto l : m_lines) {
            m_width =
                std::max<size_t>(m_width, string_actual_printed_length(l));
            ++m_lines_count;
        }
    }
    TextBlock(const std::string& s) {
        m_lines.push_back(s);
        m_width = string_actual_printed_length(s);
        m_lines_count = 1;
    }
    const Lines& lines() const { return m_lines; }
    size_t width() const { return m_width; }
    void set_width(size_t width) { m_width = width; }
    int lines_count() const { return m_lines_count; }
};

class TextBlocks : public ForwardList<TextBlock> {
   private:
    const int m_offset;
    int m_max_line_length = 0;
    friend std::ostream& operator<<(std::ostream& stream,
                                    const TextBlocks& blocks);

   public:
    TextBlocks(int offset = 3) : m_offset(offset) {}
    TextBlocks(TextBlocks&& o)
        : ::ForwardList<TextBlock>(std::move(o)),
          m_offset(o.m_offset),
          m_max_line_length(o.m_max_line_length) {}
    template <typename... Args>
    void emplace_back(Args&&... args) {
        ::ForwardList<TextBlock>::emplace_back(std::forward<Args>(args)...);
        m_max_line_length = std::max(m_max_line_length, back().lines_count());
    }
    size_t width() const {
        size_t width = 0;
        if (!empty()) {
            for (auto block = cbegin(); block != cend(); ++block)
                width += block->width() + m_offset;
        }
        return width;
    }
};
