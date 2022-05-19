#include <cmath>
#include <iostream>
#include <sstream>

#include "array.h"
#include "box_drawing_chars.h"
#include "text_block.h"

struct Peg {
    struct Disk {
        int m_number;
        Disk* m_next = nullptr;
        Peg* m_peg = nullptr;
        Disk() : Disk(0) {}
        Disk(int number) : m_number(number) {}
    };
    Disk* m_head = nullptr;
    int m_size = 0;
    void place(Disk* disk) {
        disk->m_next = m_head;
        disk->m_peg = this;
        m_head = disk;
        ++m_size;
    }
    Disk* remove() {
        auto* disk = m_head;
        m_head = disk->m_next;
        --m_size;
        return disk;
    }
};

std::ostream& operator<<(std::ostream& stream, const Peg& p) {
    for (auto* disk = p.m_head; disk; disk = disk->m_next)
        stream << disk->m_number << " ";
    return stream;
}

struct Pegs {
    Array<Peg> m_pegs;
    Array<Peg::Disk> m_disks;
    Pegs(size_t peg_count, int disk_count)
        : m_pegs(peg_count), m_disks(disk_count) {
        for (int i = disk_count - 1; i >= 0; --i) {
            m_disks.emplace(i, i + 1);
            m_pegs[0].place(&m_disks[i]);
        }
    }

    TextBlocks& to_text_blocks(TextBlocks& text_blocks) {
        int lines_count = 0;
        for (auto& peg : m_pegs)
            lines_count = std::max(lines_count, peg.m_size);
        lines_count = std::max(3, lines_count);

        for (auto& peg : m_pegs) {
            TextBlock::Lines lines;
            auto repeat = [](std::ostream& stream, int times, const char* str) {
                for (int i = 0; i < times; ++i) stream << str;
            };
            if (peg.m_size > 0) {
                for (int i = peg.m_size; i < lines_count; ++i)
                    lines.push_back("");
                for (auto* disk = peg.m_head; disk; disk = disk->m_next) {
                    std::stringstream ss;
                    repeat(ss, m_disks.size() - disk->m_number + 1, " ");
                    repeat(ss, disk->m_number * 2 + 1, "\u2586");
                    repeat(ss, m_disks.size() - disk->m_number + 1, " ");
                    lines.push_back(ss.str());
                }
            } else {
                std::stringstream ss;
                repeat(ss, 2 * m_disks.size() + 3, " ");
                lines.push_back(ss.str());
            }
            text_blocks.emplace_back(std::move(lines));
        }
        return text_blocks;
    }

    void print() {
        TextBlocks text_blocks;
        std::cout << to_text_blocks(text_blocks) << std::endl;
    }

    void shift(int disk_number, int shift) {
        auto& disk = m_disks[disk_number - 1];
        if (&disk != disk.m_peg->m_head)
            throw std::runtime_error(std::string("disk ") +
                                     std::to_string(disk_number) +
                                     " is not on top");
        int peg_index = disk.m_peg - &m_pegs[0] + shift;
        for (; peg_index < 0; peg_index += m_pegs.size())
            ;
        for (; peg_index >= static_cast<int>(m_pegs.size());
             peg_index -= m_pegs.size())
            ;
        m_pegs[peg_index].place(disk.m_peg->remove());
    }

    void print_iteration(int n, int d, int level) {
        std::string offset;
        for (int i = 0; i < level; ++i) offset += "  ";
        TextBlock::Lines lines;
        std::string hanoi_invocation = offset + "hanoi(" +
                                       std::to_string(n - 1) + ", " +
                                       std::to_string(-d) + ");";
        lines.push_back(hanoi_invocation);
        lines.push_back(offset + "shift(" + std::to_string(n) + ", " +
                        std::to_string(d) + ");");
        lines.push_back(hanoi_invocation);
        TextBlocks text_blocks;
        to_text_blocks(text_blocks);
        text_blocks.emplace_back(std::move(lines));
        std::cout << std::endl << text_blocks << std::endl;
    }

    void hanoi(int n, int d, int level = 0) {
        if (n == 0) return;
        hanoi(n - 1, -d, level + 1);
        shift(n, d);
        print_iteration(n, d, level);
        hanoi(n - 1, -d, level + 1);
    }
};

struct Rule {
    Array<int> m_marks;
    int m_h;
    Rule(int size, int h) : m_marks(size), m_h(h) { mark(0, size - 1, h); }
    void mark(int l, int r, int h) {
        if (h > 0) {
            auto middle = (l + r) / 2;
            mark(l, middle, h - 1);
            m_marks[middle] = h;
            mark(middle, r, h - 1);
        }
    }
    void print() {
        for (int i = m_h; i > -1; --i) {
            for (auto& mark : m_marks) {
                if (mark >= i)
                    std::cout << Box_drawing_chars::v_line;
                else
                    std::cout << " ";
                std::cout << " ";
            }
            std::cout << std::endl;
        }
    }
};

int main() {
    Pegs pegs(3, 5);
    pegs.print();
    pegs.hanoi(5, 1);

    std::cout << std::endl;

    Rule rule(25, 3);
    rule.print();
}
