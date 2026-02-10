#include <cstddef>
#include <stdexcept>
#include <atomic>
#include <memory>
#include <vector>

namespace getcracked 
{
    struct alignas(64) CacheLineAtomic {
        std::atomic<size_t> data;
        
        size_t load() const {
            return data.load();
        }

        void store(size_t s) {
            data.store(s);
        }
    };

    template <typename T>
    class SPSCQ
    {
    public:
        SPSCQ(size_t capacity) : m_Buffer(new T[capacity]), m_Head(0), m_Tail(0), m_Capacity(capacity) { 
            if (capacity < 2 || (capacity & (capacity - 1)) != 0)
                throw std::runtime_error("Queue capacity must be a power of 2");
        }

        ~SPSCQ() {
            delete[] m_Buffer;
        }

        SPSCQ(const SPSCQ<T>&) {
            throw std::runtime_error("SPSCQ is not copyable");
        }

        SPSCQ<T>& operator=(SPSCQ<T>) {
            throw std::runtime_error("SPSCQ is not copyable");
        }

        SPSCQ(SPSCQ<T>&& other) {
            throw std::runtime_error("SPSCQ is not movable");
        }

        SPSCQ<T>& operator=(SPSCQ<T>&&) {
            throw std::runtime_error("SPSCQ is not movable");
        }
        
        bool push(const T& item)
        {
            if (full())
                return false;
            size_t idx = m_Tail.load();
            *(m_Buffer + idx) = item;
            idx = (idx + 1) & (m_Capacity - 1);
            m_Tail.store(idx);
            return true;
        }

        bool pop(T& item)
        {
            if (empty())
                return false;
            size_t idx = m_Head.load();
            item = *(m_Buffer + idx);
            idx = (idx + 1) & (m_Capacity - 1);
            m_Head.store(idx);
            return true;
        }

        [[nodiscard]] bool full() const
        {
            size_t headIdx = m_Head.load(), tailIdx = m_Tail.load();
            return ((tailIdx + 1) & (m_Capacity - 1)) == headIdx;
        }

        [[nodiscard]] size_t size() const
        {
            size_t headIdx = m_Head.load(), tailIdx = m_Tail.load();
            return tailIdx < headIdx ? (tailIdx + m_Capacity - 1) - headIdx : tailIdx - headIdx;
        }

        [[nodiscard]] bool empty() const
        {
            return m_Head.load() == m_Tail.load();
        }

    private:
        CacheLineAtomic m_Head;
        CacheLineAtomic m_Tail;

        T* m_Buffer;
        size_t m_Capacity;
    };
}
