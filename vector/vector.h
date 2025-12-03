#pragma once

#include <stdexcept>

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