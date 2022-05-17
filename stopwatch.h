#pragma once

#include <chrono>

class Stopwatch {
   private:
    long start_;

   public:
    Stopwatch() : start_(now()) {}
    static long now() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(
                   system_clock::now().time_since_epoch())
            .count();
    }
    long read_out() { return now() - start_; }
};
