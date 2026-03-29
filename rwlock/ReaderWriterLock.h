#include <condition_variable>
#include <mutex>
#include <shared_mutex>
#include <queue>
namespace getcracked 
{
    class ReaderWriterLock
    {
    public:
        unsigned ReaderLock()
        {
            std::unique_lock lk(mut);
            cond.wait(lk, [this]{ return writers == 0; });
            shared_queue.emplace(shared_mut);
            lk.unlock();
            return shared_queue.size() - 1;
        }

        unsigned ReaderUnlock() 
        { 
            if (readers == 0)
                return 0;
            shared_queue.pop();
            return shared_queue.size();
        }

        void WriterLock() 
        {
            writers++;
            unique_queue.emplace(shared_mut);
        }

        void WriterUnlock()
        {
            std::lock_guard lk(mut);
            unique_queue.pop();
            if (--writers == 0)
                cond.notify_all();
        }

    private:
        std::shared_mutex shared_mut;
        std::mutex mut;
        std::queue<std::shared_lock<std::shared_mutex>> shared_queue;
        std::queue<std::unique_lock<std::shared_mutex>> unique_queue;
        unsigned readers = 0;
        unsigned writers = 0;
        std::condition_variable cond;
    };
}
