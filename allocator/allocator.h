#include <cstddef>
#include <limits>
#include <cstdlib>
#include <memory>
#include <new>
struct ChunkHeader {
    const static inline size_t HeaderMagicId = std::numeric_limits<size_t>::max();
    size_t Size{ HeaderMagicId };
    size_t Magic{ HeaderMagicId };
};

struct Chunk {
    ChunkHeader Header;
    std::byte* Memory{ nullptr };
};

class Allocator {
public:
    explicit Allocator(size_t capacity) : m_Capacity(capacity), m_Chunks(std::make_unique<std::byte[]>(capacity)) {}

    Allocator(size_t capacity, std::byte* allocated) : m_Capacity(capacity), m_Chunks(allocated) {}

    Allocator(const Allocator&) = delete;
    void operator=(const Allocator&) = delete;
    Allocator(Allocator&&) = delete;
    void operator=(Allocator&&) = delete;

    std::byte* Allocate(size_t capacity) {
        if ((m_Capacity - m_Used) < capacity)
            throw std::bad_alloc();
        Chunk* chunk = new (m_Chunks.get() + m_Used) Chunk{ChunkHeader{capacity}, m_Chunks.get() + m_Used + sizeof(ChunkHeader)};
        m_Used += capacity + sizeof(ChunkHeader);
        return chunk->Memory;
    }

    void Deallocate(std::byte* bytes) {
        if (!bytes)
            return;
        ChunkHeader* header = reinterpret_cast<ChunkHeader*>(bytes - sizeof(ChunkHeader));
        if (header->Magic != ChunkHeader::HeaderMagicId)
            throw std::bad_alloc();
        m_Used -= header->Size + sizeof(ChunkHeader);
        header->Size = header->Magic = 0;
    }
private:
    size_t m_Capacity;
    size_t m_Used{};
    std::unique_ptr<std::byte[]> m_Chunks;
};
