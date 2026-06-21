#include <unordered_map>
#include <list>
#include <optional>

template <typename Key, typename Value>
class LRUCache {
private:
    std::unordered_map<Key, std::_List_iterator<std::pair<Key, Value>>> m_map;
    std::list<std::pair<Key, Value>> m_list;
    size_t m_capacity;

public:
    LRUCache(std::size_t capacity) : m_capacity(capacity) {
        m_map.reserve(capacity);
    }

    void put(const Key& key, const Value& value) {
        if (contains(key)) {
            m_list.splice(m_list.begin(), m_list, m_map.at(key)); // move element to front of list
            m_list.front().second = value;
            return;
        }
        if (size() == m_capacity) {
            m_map.erase(m_list.back().first);
            m_list.pop_back();
        }
        m_list.emplace_front(key, value);
        m_map.emplace(key, m_list.begin());
    }

    std::optional<Value> get(const Key& key) {
        if (!contains(key))
            return std::nullopt;
        auto itr = m_map.at(key);
        m_list.splice(m_list.begin(), m_list, itr);
        return itr->second;
    }

    bool contains(const Key& key) const {
        return m_map.contains(key);
    }

    bool erase(const Key& key) {
        if (!contains(key))
            return false;
        auto itr = m_map.at(key);
        m_map.erase(key);
        m_list.erase(itr);
        return true;
    }

    std::size_t size() const {
        return m_map.size();
    }

    std::size_t capacity() const {
        return m_capacity;
    }
};
