#pragma once

#include <cstddef>
#include <mutex>

namespace getcracked 
{
    struct control_block
    {
        size_t count_{ };
        mutable std::mutex mutex_;

        control_block() : count_(0) {}

        control_block(int count) : count_(count) {}

        void inc() {
            std::lock_guard<std::mutex> lock(mutex_);
            count_++;
        }

        void dec() {
            std::lock_guard<std::mutex> lock(mutex_);
            if (count_ > 0) count_--;
        }
    };

    template <typename T>
    class shared_ptr
    {
    public:

        shared_ptr() : m_Ptr(new T()), m_ControlBlock(new control_block(1)) {}

        shared_ptr(T* pointer) : m_Ptr(pointer), m_ControlBlock(new control_block(1))
        {

        }

        shared_ptr(const shared_ptr& other) noexcept : m_Ptr(other.m_Ptr), m_ControlBlock(other.m_ControlBlock)
        {
            m_ControlBlock->inc();
        }

        shared_ptr& operator=(const shared_ptr& other) noexcept
        {
            if (&other == this)
                return *this;
            m_ControlBlock->dec();
            m_Ptr = other.m_Ptr;
            m_ControlBlock = other.m_ControlBlock;
            m_ControlBlock->inc();
            return *this;
        }

        shared_ptr(shared_ptr&& other) noexcept : m_Ptr(new T(std::move(*other.m_Ptr))), m_ControlBlock(other.m_ControlBlock)
        {
            other.m_Ptr = nullptr;
            other.m_ControlBlock = nullptr;
        }

        shared_ptr& operator=(shared_ptr&& other) noexcept
        {
            if (&other == this)
                return *this;
            m_ControlBlock->dec();
            m_Ptr = new T(std::move(*other.m_Ptr));
            m_ControlBlock = other.m_ControlBlock;
            other.m_Ptr = nullptr;
            other.m_ControlBlock = nullptr;
            return *this;
        }

        ~shared_ptr()
        {
            if (!m_ControlBlock) {
                delete m_Ptr;
                return;
            }
            m_ControlBlock->dec();
            if (m_ControlBlock->count_ == 0) {
                delete m_Ptr;
                delete m_ControlBlock;
            }
        }

        void reset(T* pointer)
        {
            this->~shared_ptr();
            m_Ptr = pointer;
            m_ControlBlock = new control_block();
        }


        size_t get_count() const
        {
            return m_ControlBlock ? m_ControlBlock->count_ : 0;
        }

        T* operator->() const { return m_Ptr; }
        T& operator*() const { return *m_Ptr; }
        operator bool() const noexcept { return m_Ptr != nullptr; }

    private:
        T* m_Ptr = nullptr;
        control_block* m_ControlBlock;
    };
}
