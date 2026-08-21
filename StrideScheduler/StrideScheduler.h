#include <cstdint>
#include <set>
#include <stdexcept>
#include <map>
#include <unordered_map>

namespace getcracked {

struct ScheduledProcess {
    uint32_t pid;
    uint32_t pass;
    uint32_t tickets;
};

struct cmp {
    bool operator()(const ScheduledProcess& lhs, const ScheduledProcess& rhs) const {
        if (lhs.pass == rhs.pass)
            return lhs.pid < rhs.pid;
        return lhs.pass < rhs.pass;
    }
};

class StrideScheduler {
public:
    static constexpr uint64_t STRIDE_BASE = 10000;

    void add_process(uint32_t pid, uint32_t tickets) {
        if (!tickets)
            throw std::invalid_argument("No tickets allocated");

        uint32_t min_pass = m_processes.empty() ? 0 : m_processes.begin()->pass;
        auto [itr, success] = m_processes.emplace(pid, min_pass, tickets);
        if (!success)
            throw std::invalid_argument("Process already exists");
        m_umap[pid] = itr;
    }

    void remove_process(uint32_t pid) {
        if (!m_umap.contains(pid))
            throw std::invalid_argument("Process does not exist");
        auto itr{ m_umap.at(pid) };
        m_processes.erase(itr);
        m_umap.erase(pid);
    }

    uint32_t tick() {
        if (m_processes.empty())
            throw std::runtime_error("No processes to run");
        
        ++m_ticks;
        ScheduledProcess chosen{ m_processes.extract(m_processes.begin()).value() };
        chosen.pass += STRIDE_BASE / chosen.tickets;
        uint32_t pid{ chosen.pid };
        auto itr{ m_processes.insert(std::move(chosen)).first };
        m_umap[pid] = itr;
        return pid;
    }

    uint64_t current_tick() const {
        return m_ticks;
    }

private:
    std::set<ScheduledProcess, cmp> m_processes;
    std::unordered_map<uint32_t, std::set<ScheduledProcess, cmp>::iterator> m_umap;
    uint64_t m_ticks{};
};

} // namespace getcracked
