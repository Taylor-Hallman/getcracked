#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
template <typename T>
struct DataWrapper
{
    T data{ };
    bool is_last_chunk{ false };
};

template <typename T, typename Callback>
class SPSC
{
public:
    SPSC(Callback callback) : callback(callback)
    {
        consumer = std::thread(&SPSC::Consume, this);
    }

    ~SPSC() {
        {
            // Signal consumer thread to stop in case no data with
            // is_last_chunk == true is ever pushed
            std::lock_guard lk(mut);
            stop = true;
        }
        if (consumer.joinable())
            consumer.join(); 
    }

    SPSC(SPSC&) {
        throw std::runtime_error("SPSC Queue is not copyable.");
    }

    SPSC& operator=(SPSC&) {
        throw std::runtime_error("SPSC Queue is not copyable.");
    }

    SPSC(SPSC&&) {
        throw std::runtime_error("SPSC Queue is not movable.");
    }

    SPSC& operator=(SPSC&&) {
        throw std::runtime_error("SPSC Queue is not movable.");
    }

    void PushWork(const DataWrapper<T>& wrapper)
    {
        std::lock_guard lock(mut);
        work_queue.push(std::make_shared<DataWrapper<T>>(wrapper));
        data_cond.notify_one();
    }

private:
    Callback callback;
    std::queue<std::shared_ptr<DataWrapper<T>>> work_queue;
    std::mutex mut;
    std::condition_variable data_cond;
    std::thread consumer;
    bool stop = false;

    void Consume()
    {
        while (true) {
            std::unique_lock lock(mut);
            data_cond.wait(lock, [this]{ return stop || !work_queue.empty(); });
            if (stop && work_queue.empty())
                break;
            std::shared_ptr<DataWrapper<T>> work = work_queue.front();
            work_queue.pop();
            lock.unlock();
            callback(work->data);
            if (work->is_last_chunk)
                break;
        }
    }
};
