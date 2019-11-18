#include <random>

#include "array.h"

class Random_sequence_generator {
    private:
        std::mt19937 engine;
        std::uniform_int_distribution<std::mt19937::result_type> distribution;
    public:
        Random_sequence_generator(unsigned long seed, int b, int e)
            :engine(seed), distribution(b, e)
        {}
        template<typename A>
        auto generate_array(size_t size) {
            A array(size);
            for (auto& e : array)
                e = generate();
            return array;
        }
        int generate() {
            return distribution(engine);
        }
};
