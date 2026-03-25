#include "spscq/Queue.h"
#include "util/Point.h"

#include <atomic>
#include <iostream>
#include <thread>

int main() {
    std::atomic<bool> called{ false };
    auto callback = [&called](int) {
        std::cout << "Hello!" << std::endl;
        called.store(true, std::memory_order_relaxed);
    };
    {
        SPSC<int, decltype(callback)> q(callback);
        q.PushWork({42, false});
        q.PushWork({0, false});
    }

    while (!called.load(std::memory_order_relaxed)) {
        std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
    }

    std::cout << "We said 'Hello'!" << std::endl;

    return 0;
}
