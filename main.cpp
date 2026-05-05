#include "thread_pool/ThreadPool.h"

int main() {

    ThreadPool pool;
    std::atomic<int> seen{0};

    pool.submit([&] { seen.fetch_add(1, std::memory_order_relaxed); });

    // Wait briefly for the task to run.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    const bool executed = seen.load(std::memory_order_relaxed) == 1;

    return executed ? 0 : 1;
}
