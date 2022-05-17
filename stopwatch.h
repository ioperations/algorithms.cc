#pragma once

#include <chrono>

class Stopwatch {
   private:
    long m_start;

   public:
    Stopwatch() : m_start(now()) {}
    static long now() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(
                   system_clock::now().time_since_epoch())
            .count();
    }
    long read_out() { return now() - m_start; }
};
