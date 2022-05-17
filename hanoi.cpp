#include <cmath>
#include <iostream>
#include <sstream>

#include "array.h"
#include "box_drawing_chars.h"
#include "text_block.h"

struct Peg {
    struct Disk {
        int number_;
        Disk* next_ = nullptr;
        Peg* peg_ = nullptr;
        Disk() : Disk(0) {}
        Disk(int number) : number_(number) {}
    };
    Disk* head_ = nullptr;
    int size_ = 0;
    void place(Disk* disk) {
        disk->next_ = head_;
        disk->peg_ = this;
        head_ = disk;
        ++size_;
    }
    Disk* remove() {
        auto disk = head_;
        head_ = disk->next_;
        --size_;
        return disk;
    }
};

std::ostream& operator<<(std::ostream& stream, const Peg& p) {
    for (auto disk = p.head_; disk; disk = disk->next_)
        stream << disk->number_ << " ";
    return stream;
}

struct Pegs {
    Array<Peg> pegs_;
    Array<Peg::Disk> disks_;
    Pegs(size_t peg_count, int disk_count)
        : pegs_(peg_count), disks_(disk_count) {
        for (int i = disk_count - 1; i >= 0; --i) {
            disks_.emplace(i, i + 1);
            pegs_[0].place(&disks_[i]);
        }
    }

    Text_blocks& to_text_blocks(Text_blocks& text_blocks) {
        int lines_count = 0;
        for (auto& peg : pegs_) lines_count = std::max(lines_count, peg.size_);
        lines_count = std::max(3, lines_count);

        for (auto& peg : pegs_) {
            Text_block::Lines lines;
            auto repeat = [](std::ostream& stream, int times, const char* str) {
                for (int i = 0; i < times; ++i) stream << str;
            };
            if (peg.size_ > 0) {
                for (int i = peg.size_; i < lines_count; ++i)
                    lines.push_back("");
                for (auto disk = peg.head_; disk; disk = disk->next_) {
                    std::stringstream ss;
                    repeat(ss, disks_.size() - disk->number_ + 1, " ");
                    repeat(ss, disk->number_ * 2 + 1, "\u2586");
                    repeat(ss, disks_.size() - disk->number_ + 1, " ");
                    lines.push_back(ss.str());
                }
            } else {
                std::stringstream ss;
                repeat(ss, 2 * disks_.size() + 3, " ");
                lines.push_back(ss.str());
            }
            text_blocks.emplace_back(std::move(lines));
        }
        return text_blocks;
    }

    void print() {
        Text_blocks text_blocks;
        std::cout << to_text_blocks(text_blocks) << std::endl;
    }

    void shift(int disk_number, int shift) {
        auto& disk = disks_[disk_number - 1];
        if (&disk != disk.peg_->head_)
            throw std::runtime_error(std::string("disk ") +
                                     std::to_string(disk_number) +
                                     " is not on top");
        int peg_index = disk.peg_ - &pegs_[0] + shift;
        for (; peg_index < 0; peg_index += pegs_.size())
            ;
        for (; peg_index >= static_cast<int>(pegs_.size());
             peg_index -= pegs_.size())
            ;
        pegs_[peg_index].place(disk.peg_->remove());
    }

    void print_iteration(int n, int d, int level) {
        std::string offset;
        for (int i = 0; i < level; ++i) offset += "  ";
        Text_block::Lines lines;
        std::string hanoi_invocation = offset + "hanoi(" +
                                       std::to_string(n - 1) + ", " +
                                       std::to_string(-d) + ");";
        lines.push_back(hanoi_invocation);
        lines.push_back(offset + "shift(" + std::to_string(n) + ", " +
                        std::to_string(d) + ");");
        lines.push_back(hanoi_invocation);
        Text_blocks text_blocks;
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
    Array<int> marks_;
    int h_;
    Rule(int size, int h) : marks_(size), h_(h) { mark(0, size - 1, h); }
    void mark(int l, int r, int h) {
        if (h > 0) {
            auto middle = (l + r) / 2;
            mark(l, middle, h - 1);
            marks_[middle] = h;
            mark(middle, r, h - 1);
        }
    }
    void print() {
        for (int i = h_; i > -1; --i) {
            for (auto& mark : marks_) {
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
