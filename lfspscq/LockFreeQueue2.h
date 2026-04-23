#include <atomic>
#include <stdexcept>

namespace getcracked {
    template<typename T>
    class SPSCQ {
    public:
        SPSCQ(size_t capacity) : m_Data(new T[capacity]), m_Capacity(capacity) {
            if (capacity < 2 || (capacity & (capacity - 1)) != 0)
                throw std::runtime_error("Capacity must be a power of 2");
        }

        ~SPSCQ() {
            delete[] m_Data;
        }

        SPSCQ(const SPSCQ<T>&) {
            throw std::runtime_error("SPSCQ is not copyable");
        }

        SPSCQ<T>& operator=(SPSCQ<T>&) {
            throw std::runtime_error("SPSCQ is not copyable");
        }

        SPSCQ(SPSCQ<T>&&) {
            throw std::runtime_error("SPSCQ is not movable");
        }

        SPSCQ<T>& operator=(SPSCQ<T>&&) {
            throw std::runtime_error("SPSCQ is not movable");
        }

        bool push(const T& item) {
            if (full())
                return false;
            size_t const writeIdx = m_WriteIdx.load(std::memory_order_relaxed);
            size_t const nextWriteIdx = (writeIdx + 1) & (m_Capacity - 1);
            *(m_Data + writeIdx) = item;
            m_WriteIdx.store(nextWriteIdx, std::memory_order_release);
            return true;
        }

        bool pop(T& item) {
            if (empty())
                return false;
            size_t const readIdx = m_ReadIdx.load(std::memory_order_relaxed);
            size_t const nextReadIdx = (readIdx + 1) & (m_Capacity - 1);
            item = *(m_Data + readIdx);
            m_ReadIdx.store(nextReadIdx, std::memory_order_release);
            return true;
        }

        [[nodiscard]] bool full() const {
            size_t const writeIdx = m_WriteIdx.load(std::memory_order_relaxed);
            size_t const nextWriteIdx = (writeIdx + 1) & (m_Capacity - 1);
            if (nextWriteIdx == m_ReadIdxCached) {
                m_ReadIdxCached = m_ReadIdx.load(std::memory_order_acquire);
                if (nextWriteIdx == m_ReadIdxCached)
                    return true;
            }
            return false;
        }

        [[nodiscard]] bool empty() const {
            size_t const readIdx = m_ReadIdx.load(std::memory_order_relaxed);
            if (readIdx == m_WriteIdxCached) {
                m_WriteIdxCached = m_WriteIdx.load(std::memory_order_acquire);
                if (readIdx == m_WriteIdxCached)
                    return true;
            }
            return false;
        }

        [[nodiscard]] size_t size() const {
            size_t const readIdx = m_ReadIdx.load(std::memory_order_relaxed);
            size_t const writeIdx = m_WriteIdx.load(std::memory_order_relaxed);
            return writeIdx < readIdx ? (writeIdx + m_Capacity - 1) - readIdx : writeIdx - readIdx;
        }
        
    private:
        T* m_Data;
        size_t m_Capacity;
        alignas(64) std::atomic<size_t> m_ReadIdx{0};
        alignas(64) mutable size_t m_WriteIdxCached{0};
        alignas(64) std::atomic<size_t> m_WriteIdx{0};
        alignas(64) mutable size_t m_ReadIdxCached{0};
    };
}
