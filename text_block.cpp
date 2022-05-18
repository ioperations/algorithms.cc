#include "text_block.h"

std::ostream& operator<<(std::ostream& stream, const TextBlocks& blocks) {
    ForwardList<TextBlock::Lines::const_iterator> text_block;
    for (auto block = blocks.cbegin(); block != blocks.cend(); ++block)
        text_block.push_back(block->lines().cbegin());

    for (int line = 0; line < blocks.m_max_line_length; ++line) {
        auto block = blocks.cbegin();
        for (auto& entry : text_block) {
            if (!entry.empty()) {
                std::cout << (*entry);
                for (size_t i = 0;
                     i < block->width() - string_actual_printed_length(*entry);
                     ++i)
                    std::cout << " ";
                ++entry;
            } else {
                for (size_t i = 0; i < block->width(); ++i) std::cout << " ";
            }
            for (int i = 0; i < blocks.m_offset; ++i) std::cout << " ";
            ++block;
        }
        if (line < blocks.m_max_line_length - 1) std::cout << std::endl;
    }
    return stream;
}
