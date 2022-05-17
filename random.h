#include <random>

#include "array.h"

template <typename T>
class RandomSequenceGenerator {
   private:
    std::mt19937 engine;
    std::uniform_int_distribution<std::mt19937::result_type> distribution;
    int m_size;

   public:
    RandomSequenceGenerator(unsigned long seed, T b, T e)
        : engine(seed), distribution(b, e), m_size(e - b) {}

    template <typename A>
    auto generate_array() {
        return generate_array<A>(m_size);
    }

    template <typename A>
    auto generate_array(size_t size) {
        A array(size);
        for (auto& e : array) e = generate();
        return array;
    }

    T generate() { return distribution(engine); }
};
