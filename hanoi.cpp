#include <iostream> 
#include <sstream>

#include "array.h"
#include "text_block.h"

struct Peg {
    struct Disk {
        int number_;
        Disk* next_ = nullptr;
        Peg* peg_ = nullptr;
        Disk() :Disk(0) {}
        Disk(int number) :number_(number) {}
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
    Pegs(size_t peg_count, int disk_count) :pegs_(peg_count), disks_(disk_count) {
        for (int i = disk_count - 1; i >= 0; --i) {
            disks_.emplace(i, i + 1);
            pegs_[0].place(&disks_[i]);
        }
    }
    void print() {
        int lines_count = 0;
        for (auto& peg : pegs_)
            lines_count = std::max(lines_count, peg.size_);
        ++lines_count;

        Text_blocks text_blocks;
        for (auto& peg : pegs_) {
            Text_block::Lines lines;

            for (int i = peg.size_; i < lines_count; ++i)
                lines.push_back("");
            auto repeat = [](std::ostream& stream, int times, const char* str) {
                for (int i = 0; i < times; ++i)
                    stream << str;
            };
            if (peg.size_ > 0)
                for (auto disk = peg.head_; disk; disk = disk->next_) {
                    std::stringstream ss;
                    repeat(ss, disks_.size() - disk->number_ + 1, " ");
                    repeat(ss, disk->number_ * 2 + 1, "\u2586");
                    repeat(ss, disks_.size() - disk->number_ + 1, " ");
                    lines.push_back(ss.str());
                }
            else {
                std::stringstream ss;
                repeat(ss, 2 * disks_.size() + 3, " ");
                lines.push_back(ss.str());
            }
            text_blocks.emplace_back(std::move(lines));
        }
        std::cout << text_blocks << std::endl;
    }
    void shift(int disk_number, int shift) {
        std::cout << disk_number << " " << shift << std::endl;
        auto& disk = disks_[disk_number - 1];
        if (&disk != disk.peg_->head_)
            throw std::runtime_error(std::string("disk ") + std::to_string(disk_number) + " is not on top");
        auto peg_index = disk.peg_ - &pegs_[0] + shift;
        if (peg_index < 0)
            peg_index = pegs_.size() + peg_index;
        pegs_[peg_index].place(disk.peg_->remove());
        print();
    }
    void hanoi(int n, int d) {
        if (n == 0) return;
        hanoi(n - 1, -d);
        shift(n, d);
        hanoi(n - 1, -d);
    }
};


int main() {
    Pegs pegs(3, 5);
    pegs.print();
    pegs.hanoi(3, 1);
    // pegs.hanoi(3, 1);
    // pegs.shift(1, 1);
    // pegs.print();
    // pegs.shift(2, 2);
    // pegs.print();
    // pegs.shift(3, -1);
    // pegs.print();
}
