#include "pool_allocator/PoolAllocator.h"
#include <iostream>

int main() {
    // Use Case 1: allocate two messages, then read one back in place through its handle
    MessagePool pool(4);
    int a = pool.allocate({10, 64});
    int b = pool.allocate({11, 128});
    std::cout << a << " " << b << "\n";
    // Output: 0 1
    std::cout << pool.get(b)->id << " " << pool.get(b)->size << "\n";
    // Output: 11 128

    MessagePool pool2(3);
    pool2.allocate({1, 10});
    pool2.allocate({2, 20});
    pool2.allocate({3, 30});
    pool2.release(0);
    pool2.release(2);
    std::cout << pool2.allocate({9, 90}) << "\n";
    // Output: 2

    // Use Case 3: an exhausted pool refuses, and a released handle is dead
    MessagePool pool3(1);
    int h = pool3.allocate({7, 70});
    std::cout << pool3.allocate({8, 80}) << "\n";
    // Output: -1
    std::cout << pool3.release(h) << " " << pool3.release(h) << "\n";
    // Output: 1 0
    std::cout << (pool3.get(h) == nullptr) << " " << pool3.available() << "\n";
    // Output: 1 1
}
