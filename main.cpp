#include "3DArray/heap_array.h"
#include <iostream>

int main() {
    size_t x = 1uz, y = 2uz, z = 3uz;
    int*** arr = GetSpace(x, y, z);
    std::cout << "Done\n";
    for (size_t i{}; i < x; ++i)
        for (size_t j{}; j < y; ++j)
            for (size_t k{}; k < z; ++k)
                std::cout << arr[i][j][k] << ", ";
    std::cout << "\n";
}
