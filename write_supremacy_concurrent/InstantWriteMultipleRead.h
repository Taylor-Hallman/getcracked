#include <atomic>
#include <memory>

namespace getcracked
{
    template <typename Element>
    class InstantWriteMultipleRead
    {
    public:
        void Write(const Element& value)
        {
            unsigned next = m_WriteIdx ^ 1;
            m_Buf[next] = value;
            m_Value.store(&m_Buf[next], std::memory_order_release);
            m_WriteIdx = next;
        }
        
        bool Read(Element& out) const
        {
            Element* val = m_Value.load(std::memory_order_acquire);
            if (!val)
                return false;
            out = *val;
            return true;
        }

    private:
        alignas(64) Element m_Buf[2];
        std::atomic<Element*> m_Value{nullptr};
        unsigned m_WriteIdx{0};
    };
}
