#include "array.h"

template<>
Array<bool> Array_builder<Array<bool>, bool>::build() {
    Array<bool> a(count_);
    int index = -1;
    for (bool b : list_) { // todo optimize?
        a[++index] = b;
    }
    return a;
}

