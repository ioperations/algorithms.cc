#include <random>

#include "array.h"

template<typename T>
class Random_sequence_generator {
    private:
        std::mt19937 engine;
        std::uniform_int_distribution<std::mt19937::result_type> distribution;
        int size_;
    public:
        Random_sequence_generator(unsigned long seed, T b, T e)
            :engine(seed), distribution(b, e), size_(e - b)
        {}

        template<typename A>
        auto generate_array() { return generate_array<A>(size_); }

        template<typename A>
        auto generate_array(size_t size) {
            A array(size);
            for (auto& e : array)
                e = generate();
            return array;
        }

        T generate() {
            return distribution(engine);
        }
};
