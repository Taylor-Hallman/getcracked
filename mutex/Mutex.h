#include <atomic>
#include <thread>

class Mutex
{
public:
    void lock()
    {
        int cycles = 0;
        while (flag.exchange(true, std::memory_order_acq_rel)) {
            if (!(cycles = (cycles + 1) & 3))
                std::this_thread::yield();
        }
    }

    void unlock()
    {
        flag.store(false, std::memory_order_acquire);
    }
private:
    std::atomic<bool> flag = false;
};
