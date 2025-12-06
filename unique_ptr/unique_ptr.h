#pragma once

#include <stdexcept>
#include <memory>

namespace getcracked 
{
    template <typename T>
    struct custom_deleter
    {
        void operator()(T* pointer) const
        {
            delete pointer;
        }
    };

    template <typename T, typename custom_deleter = custom_deleter<T>>
    class unique_ptr
    {
    public:
        unique_ptr()
        {
            m_Pointer = new T();
        }
        unique_ptr(T* pointer) : m_Pointer(pointer)
        {
        }

        unique_ptr(const unique_ptr&) {
            throw std::runtime_error("Cannot copy a unique ptr");
        };
        unique_ptr& operator=(const unique_ptr&) {
            throw std::runtime_error("Cannot copy a unique ptr");
        };

        unique_ptr(unique_ptr&& other) noexcept : m_Pointer(other.release())
        {
        }

        unique_ptr& operator=(unique_ptr&& other) noexcept
        {
            delete m_Pointer;
            m_Pointer = other.release();
            return *this;
        }

        ~unique_ptr()
        {
            custom_deleter()(m_Pointer);
        }

        T* release()
        {
            T* newPointer = new T(std::move(*m_Pointer));
            delete this;
            return newPointer;
        }

        void reset(T* pointer = nullptr)
        {
            custom_deleter()(m_Pointer);
            m_Pointer = pointer;
        }

        bool is_owning() const { return m_Pointer != nullptr; }


        T& operator*() const {
            return *m_Pointer;
        }
        T* operator->() const {
            return m_Pointer;
        }
        operator bool() const {
            return is_owning();
        }

    private:
        T* m_Pointer = nullptr;
    };

    template<typename T, typename... ArgTypes>
    std::unique_ptr<T> make_unique(ArgTypes&&... Args) {
      return std::unique_ptr<T>(new T(std::forward<ArgTypes>(Args)...));
   }
}