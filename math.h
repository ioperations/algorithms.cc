#pragma once

/**
 * Divides integers rounding up.
 */
template<typename T>
T divide_round_up(T dividend, T divisor) {
    T mod = 0;
    for (; dividend > divisor; dividend -= divisor, ++mod);
    if (divisor - dividend != divisor) 
        ++mod;
    return mod;
}
