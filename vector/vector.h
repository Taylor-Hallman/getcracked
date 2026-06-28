#pragma once

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <new>
#include <type_traits>

#if 0
namespace getcracked {
    template<typename vector>
    class VectorIter {
    public:
        using ValueType = typename vector::ValueType;
        using PointerType = ValueType*;
        using ReferenceType = ValueType&;
    public:
        VectorIter(PointerType ptr) : m_Ptr(ptr) {}

        VectorIter& operator++() {
            m_Ptr++;
            return *this;
        }

        VectorIter operator++(int) {
            VectorIter itr = *this;
            ++(*this);
            return itr;
        }

        VectorIter& operator--() {
            m_Ptr--;
            return *this;
        }

        VectorIter operator--(int) {
            VectorIter itr = *this;
            --(*this);
            return itr;
        }

        ReferenceType operator[](int index) {
            return *(m_Ptr + index);
        }

        PointerType operator->() {
            return m_Ptr;
        }

        ReferenceType operator*() {
            return *m_Ptr;
        }

        bool operator==(const VectorIter& other) const {
            return m_Ptr == other.m_Ptr;
        }

        bool operator!=(const VectorIter& other) const {
            return !(*this == other);
        }
    private:
        PointerType m_Ptr;
    };

    template <typename Element>
    class vector {
    public:
        using ValueType = Element;
        using Iterator = VectorIter<vector<Element>>;
    public:
        void push_back(Element element) {
            m_Size++;
            if (m_Size >= m_Capacity) {
                ReAlloc(m_Capacity * 3);
            }
            m_Data[m_Size - 1] = element;
        }
        const Element& at(size_t index) const {
            if (index >= m_Size)
                throw std::out_of_range("Index out of range");
            return m_Data[index];
        }
        size_t get_size() const {
            return m_Size;
        }
        size_t get_capacity() const {
            return m_Capacity;
        }
        void shrink_to_fit() {
            if (m_Capacity == m_Size)
                return;
            ReAlloc(m_Size);
        }
        void pop_back() {
            m_Size--;
            m_Data[m_Size].~Element();
        }

        Iterator begin() {
            return m_Data;
        }
        Iterator end() {
            return (m_Data + m_Size);
        }
    private:
        // Data
        Element* m_Data = nullptr;
        size_t m_Size = 0;
        size_t m_Capacity = 1;

        void ReAlloc(size_t newCapacity) {
            Element* newData = (Element*)::operator new(sizeof(Element) * newCapacity);

            if (newCapacity < m_Size)
                m_Size = newCapacity;

            if (m_Data) {
                for (int i = 0; i < m_Size; i++) {
                    newData[i] = std::move(m_Data[i]);
                    m_Data[i].~Element();
                }
            }

            ::operator delete(m_Data, m_Capacity * sizeof(Element));
            m_Data = newData;
            m_Capacity = newCapacity;
        }
    };
}
#else
namespace getcracked {
    template <typename Element>
    class vector {
    public:
        vector() : m_data{ static_cast<Element*>(operator new(sizeof(Element))) } {}

        void push_back(const Element& element) {
            new (&m_data[m_size++]) Element(std::move(element));
            if (m_size == m_capacity)
                re_alloc(m_capacity * growth_factor);
        }
        const Element& at(std::size_t index) const {
            if (index >= m_size)
                throw std::out_of_range("Index out of range");
            return m_data[index];
        }
        std::size_t get_size() const {
            return m_size;
        }
        std::size_t get_capacity() const {
            return m_capacity;
        }
        void shrink_to_fit() {
            re_alloc(m_size);
        }
        void pop_back() {
            m_data[--m_size].~Element();
        }
        ~vector() {
            std::destroy(m_data, m_data + m_size);
            operator delete(m_data);
        }
    private:
        Element* m_data;
        size_t m_size{};
        size_t m_capacity{1uz};

        void re_alloc(size_t new_capacity) {
            if (new_capacity < m_size)
                m_size = new_capacity;
            Element* buf = static_cast<Element*>(operator new(sizeof(Element) * new_capacity));

            if (m_data) {
                std::uninitialized_copy(m_data, m_data + m_size, buf);
                if constexpr (!std::is_trivially_destructible_v<Element>)
                    std::destroy(m_data, m_data + m_size);
            }

            operator delete(m_data);
            m_data = buf;
            m_capacity = new_capacity;
        }

        static constexpr size_t growth_factor{3uz};
    };
}
#endif
