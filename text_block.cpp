#include "text_block.h"

std::ostream& operator<<(std::ostream& stream, const Text_blocks& blocks) {
    Forward_list<Text_block::Lines::const_iterator> text_block;
    for (auto block = blocks.cbegin(); block != blocks.cend(); ++block)
        text_block.push_back(block->lines().cbegin());

    for (int line = 0; line < blocks.max_line_length_; ++line) {
        auto block = blocks.cbegin();
        for (auto& entry : text_block) {
            if (!entry.empty()) {
                std::cout << (*entry);
                for (size_t i = 0; i < block->width() - string_length(*entry); ++i)
                    std::cout << " ";
                ++entry;
            } else {
                for (size_t i = 0; i < block->width(); ++i)
                    std::cout << " ";
            }
            for (int i = 0; i < blocks.offset_; ++i)
                std::cout << " ";
            ++block;
        }
        if (line < blocks.max_line_length_ - 1)
            std::cout << std::endl;
    }
    return stream;
}

