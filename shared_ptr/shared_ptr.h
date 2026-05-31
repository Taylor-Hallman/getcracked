// Write your solution here
// C++23 using GCC 14.2
// Debug with std::cerr or std::clog.
// !!! IMPORTANT !!!
// 99% of headers are pre-compiled for you server-side.
// If your submission fails to compile due to a missing header, add it to your submission.

#include <cstddef>
#include <mutex>

namespace getcracked 
{
    struct control_block
    {
        size_t count_{ };
        mutable std::mutex mutex_;
    };

    template <typename T>
    class shared_ptr
    {
    public:

        shared_ptr() : m_Ptr{ nullptr }, m_Cb{ new control_block() } { }
        shared_ptr(T* pointer) : m_Ptr{ pointer }, m_Cb(new control_block{ 1 })
        {
        }

        shared_ptr(const shared_ptr& other) noexcept : m_Ptr(other.m_Ptr), m_Cb(other.m_Cb)
        {
            std::scoped_lock lock(m_Cb->mutex_);
            ++m_Cb->count_;
        }

        shared_ptr& operator=(const shared_ptr& other) noexcept
        {
            if (this == &other)
                return *this;
            if (m_Ptr) {
                delete m_Ptr;
                std::scoped_lock lock(m_Cb->mutex_);
                --m_Cb->count_;
            }
            else
                delete m_Cb;
            m_Ptr = other.m_Ptr;
            m_Cb = other.m_Cb;
            {
                std::scoped_lock lock(m_Cb->mutex_);
                ++m_Cb->count_;
            }
            return *this;
        }

        shared_ptr(shared_ptr&& other) noexcept : m_Ptr(other.m_Ptr), m_Cb(other.m_Cb)
        {
            other.m_Ptr = nullptr;
            other.m_Cb = nullptr;
        }

        shared_ptr& operator=(shared_ptr&& other) noexcept
        {
            if (this == &other)
                return *this;
            if (m_Ptr) {
                delete m_Ptr;
                std::scoped_lock lock(m_Cb->mutex_);
                --m_Cb->count_;
                if (!m_Cb->count_) {
                    delete m_Ptr;
                    delete m_Cb;
                }
            }
            else
                delete m_Cb;
            m_Ptr = other.m_Ptr;
            m_Cb = other.m_Cb;
            other.m_Ptr = nullptr;
            other.m_Cb = nullptr;
            return *this;
        }

        ~shared_ptr()
        {
            if (!m_Cb)
                return;
            if (!m_Cb->count_) {
                delete m_Cb;
                return;
            }
            {
               std::scoped_lock lock(m_Cb->mutex_);
                --m_Cb->count_;
            }
            if (!m_Cb->count_) {
                delete m_Ptr;
                delete m_Cb;
            }
        }

        void reset(T* pointer)
        {
            if (m_Cb->count_) {
                std::scoped_lock lock(m_Cb->mutex_);
                --m_Cb->count_;
            }
            if (!m_Cb->count_) {
                delete m_Ptr;
                delete m_Cb;
            }
            m_Ptr = pointer;
            m_Cb = new control_block();
        }


        size_t get_count() const
        {
            if (!m_Cb)
                return 0;
            return m_Cb->count_;
        }

        T* operator->() const { 
            return m_Ptr;
        }
        T& operator*() const { 
            return *m_Ptr;
        }
        operator bool() const noexcept { 
            return m_Ptr != nullptr;
        }

    private:
        T* m_Ptr;
        control_block* m_Cb;
    };
}
