#pragma once

#include <array>

namespace Std_ext {

template <typename T, std::size_t SZ, std::size_t I = 0>
inline void fill_array(std::array<T, SZ>& array) {}

template <typename T, std::size_t SZ, std::size_t I = 0, typename... Args>
inline void fill_array(std::array<T, SZ>& array, T&& t, Args&&... args) {
    array[I] = t;
    fill_array<T, SZ, I + 1>(array, std::forward<Args>(args)...);
}

template <typename T, typename... Args, std::size_t SZ = sizeof...(Args) + 1>
inline auto make_array(T&& t, Args&&... args) {
    std::array<T, SZ> array;
    fill_array<T, SZ>(array, std::forward<T>(t), std::forward<Args>(args)...);
    return array;
}

template <typename T, std::size_t SZ, std::size_t I>
inline void fill_pointers_array(std::array<T*, SZ>& array) {}

template <typename T, std::size_t SZ, std::size_t I, typename... Args>
inline void fill_pointers_array(std::array<T*, SZ>& array, T& t,
                                Args&... args) {
    array[I] = &t;
    fill_pointers_array<T, SZ, I + 1>(array, args...);
}

template <typename T, typename... Args, std::size_t SZ = sizeof...(Args) + 1>
inline auto make_pointers_array(T& t, Args&... args) {
    std::array<T*, SZ> array;
    fill_pointers_array<T, SZ, 0>(array, t, args...);
    return array;
}

template <typename T, std::size_t SZ, std::size_t I = 0, typename F>
inline typename std::enable_if<I == SZ, void>::type for_each(
    const std::array<T, SZ>& array, F f) {}

template <typename T, std::size_t SZ, std::size_t I = 0, typename F>
    inline typename std::enable_if <
    I<SZ, void>::type for_each(const std::array<T, SZ>& array, F f) {
    f(array[I]);
    for_each<T, SZ, I + 1>(array, f);
}

}  // namespace Std_ext
