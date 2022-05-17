#pragma once
#include "forward_list.h"
#include "string_utils.h"

class Text_block {
   public:
    using Lines = Forward_list<std::string>;

   private:
    Lines lines_;
    size_t width_ = 0;
    int lines_count_ = 0;

   public:
    Text_block(Lines&& lines) : lines_(std::move(lines)) {
        for (auto l : lines_) {
            width_ = std::max<size_t>(width_, string_actual_printed_length(l));
            ++lines_count_;
        }
    }
    Text_block(const std::string& s) {
        lines_.push_back(s);
        width_ = string_actual_printed_length(s);
        lines_count_ = 1;
    }
    const Lines& lines() const { return lines_; }
    size_t width() const { return width_; }
    void set_width(size_t width) { width_ = width; }
    int lines_count() const { return lines_count_; }
};

class Text_blocks : public Forward_list<Text_block> {
   private:
    const int offset_;
    int max_line_length_ = 0;
    friend std::ostream& operator<<(std::ostream& stream,
                                    const Text_blocks& blocks);

   public:
    Text_blocks(int offset = 3) : offset_(offset) {}
    Text_blocks(Text_blocks&& o)
        : Forward_list<Text_block>(std::move(o)),
          offset_(o.offset_),
          max_line_length_(o.max_line_length_) {}
    template <typename... Args>
    void emplace_back(Args&&... args) {
        Forward_list<Text_block>::emplace_back(std::forward<Args>(args)...);
        max_line_length_ = std::max(max_line_length_, back().lines_count());
    }
    size_t width() const {
        size_t width = 0;
        if (!empty()) {
            for (auto block = cbegin(); block != cend(); ++block)
                width += block->width() + offset_;
        }
        return width;
    }
};
