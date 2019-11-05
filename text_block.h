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
        Text_block(Lines&& lines): lines_(std::move(lines)) {
            for (auto l : lines_) {
                width_ = std::max<size_t>(width_, string_length(l));
                ++lines_count_;
            }
        }
        const Lines& lines() const { return lines_; }
        size_t width() const { return width_; }
        int lines_count() const { return lines_count_; }
};

class Text_blocks : protected Forward_list<Text_block> {
    private:
        int max_line_length_ = 0;
        friend std::ostream& operator<<(std::ostream& stream, const Text_blocks& blocks);
    public:
        template<typename... Args>
            void emplace_back(Args&&... args) {
                Forward_list<Text_block>::emplace_back(std::forward<Args>(args)...);
                max_line_length_ = std::max(max_line_length_, back().lines_count());
            }
};

