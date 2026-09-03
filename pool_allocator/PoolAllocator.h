#include <cassert>
#include <list>
#include <numeric>
#include <unordered_map>
#include <variant>
#include <vector>

struct Message {
    int id;
    int size;
};

class MessagePool {
public:
    MessagePool(int capacity) : m_available{ capacity } {
        m_slots.resize(capacity);
        std::iota(m_slots.begin(), m_slots.end() - 1, 1);
        m_slots[capacity - 1] = -1;
    }

    int allocate(Message message) {
        if (m_head < 0)
            return -1;

        --m_available;
        auto& head{ m_slots[m_head] };
        int nextFree{ std::get<1>(head) };
        assert(nextFree == -1 || m_slots[nextFree].index() == 1);
        
        int insertedIdx{ m_head };
        m_head = nextFree;
        head = message;
        return insertedIdx;
    }

    bool release(int handle) {
        if (handle < 0 || handle >= m_slots.size())
            return false;
        if (m_slots[handle].index() == 1)
            return false;

        m_slots[handle] = m_head;
        m_head = handle;
        ++m_available;
        return true;
    }

    Message* get(int handle) {
        if (handle < 0 || handle >= m_slots.size())
            return nullptr;
        return std::get_if<Message>(&m_slots[handle]);
    }

    int available() const {
        return m_available;
    }

    void reset() {
        std::iota(m_slots.begin(), m_slots.end() - 1, 1);
        m_slots[m_slots.size() - 1] = -1;
        m_available = m_slots.size();
        m_head = 0;
    }

private:
    std::vector<std::variant<Message, int>> m_slots;
    int m_head{};
    int m_available;
};
