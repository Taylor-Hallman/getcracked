#include <unordered_map>
#include <list>
#include <string>
#include <optional>

struct Instrument { 
    std::string Symbol; 
    double Price;

    bool operator==(const Instrument& other) const {
        return Symbol == other.Symbol;
    }
};

namespace std {
    template<>
    struct hash<Instrument> {
        size_t operator()(const Instrument& instr) const {
            return std::hash<std::string>()(instr.Symbol);
        }
    };
}

template <typename T>
class getcrackedQueue
{
public:

    void write(const T& item)
    {
        if (m_map.contains(item)) {
            auto itr = m_map.at(item);
            *itr = item;
            return;
        }
        m_items.push_back(item);
        auto itr = m_items.end();
        std::advance(itr, -1);
        m_map.emplace(item, itr);
        ++m_size;
    }

    std::optional<T> read()
    {
        if (empty())
            return std::nullopt;
        T item = m_items.front();
        m_map.erase(item);
        m_items.pop_front();
        --m_size;
        return item;
    }

    bool try_read(T& item)
    {
        if (empty())
            return false;
        item = m_items.front();
        m_map.erase(item);
        m_items.pop_front();
        --m_size;
        return true;
    }

    [[nodiscard]] bool empty() const
    {
        return !size();
    }

    [[nodiscard]] std::size_t size() const
    {
        return m_size;
    }


private:
    std::unordered_map<T, typename std::list<T>::iterator> m_map;
    std::list<T> m_items;
    size_t m_size{};
};
