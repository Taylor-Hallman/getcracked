#include <cstddef>
#include <new>
template <size_t Capacity>
class StackAllocator {
public:
    StackAllocator() : m_Offset(0) {}

    void* alloc(size_t size, size_t align = alignof(std::max_align_t)) {
        size_t misalign = m_Offset & (align - 1);
        size_t padding = misalign ? align - misalign : 0;
        if (size + padding > remaining())
            return nullptr;
        void* mem = static_cast<void*>(m_Buf + m_Offset + padding);
        m_Offset += padding + size;
        return mem;
    }

    void reset() {
        m_Offset = 0;
    }

    size_t remaining() const {
        return Capacity - m_Offset;
    }

private:
    size_t m_Offset;
    std::byte m_Buf[Capacity];
};
