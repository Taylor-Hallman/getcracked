#pragma once

#include <algorithm>
#include <cstddef>
#include <limits>
int*** GetSpace(size_t x, size_t y, size_t z) {
    int*** arr = new int**[x];
    for (size_t i{}; i < x; ++i) {
        arr[i] = new int*[y];
        for (size_t j{}; j < y; ++j) {
            arr[i][j] = new int[z];
            std::fill(arr[i][j], arr[i][j] + z, std::numeric_limits<int>::min());
        }
    }
    return arr;
}
