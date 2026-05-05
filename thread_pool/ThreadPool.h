#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <vector>
#include <thread>
#include <cassert>

class ThreadPool
{
public:
    ThreadPool() {
        size_t count = std::thread::hardware_concurrency();
        threads.reserve(count);
        if (!count) count = 2;
        while (threads.size() < count)
            threads.emplace_back(&ThreadPool::Work, this);
    }

    ThreadPool(std::size_t count)
    {
        threads.reserve(count);
        while (threads.size() < count)
            threads.emplace_back(&ThreadPool::Work, this);
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ~ThreadPool()
    {
        shutdown.store(true, std::memory_order_relaxed);
        for (std::thread& t : threads) {
            if (t.joinable())
                t.join();
        }
    }

    size_t ThreadCount() const 
    {
        return threads.size();
    }

    template <typename Work>
    void submit(Work work)
    {
        {
            std::lock_guard lock(mut);
            tasks.push(std::function<void()>(work));
        }
    }

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::atomic<bool> shutdown{false};
    std::mutex mut;
    //std::condition_variable cv;

    void Work()
    {
        while (!shutdown.load(std::memory_order_relaxed)) {
            std::function<void()> task;
            {
                std::lock_guard lock(mut);
                if (tasks.empty()) {
                    std::this_thread::yield();
                    continue;
                }
                task = tasks.front();
                tasks.pop();
            }
            try {
                task();
            }
            catch (std::runtime_error err) {
                std::cout << err.what();
            }
        }
    }
};
