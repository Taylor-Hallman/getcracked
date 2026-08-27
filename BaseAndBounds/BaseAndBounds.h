#include <cstdint>
#include <vector>

struct ProcessInfo {
    uint32_t pid;
    uint64_t base;
    uint64_t bounds;
};

class RegionChecker {
private:
    std::vector<ProcessInfo> m_processes;
public:
    void add_process(uint32_t pid, uint64_t base, uint64_t bounds) {
        m_processes.emplace_back(pid, base, bounds);
    }

    std::vector<std::pair<uint32_t, uint32_t>> find_overlaps() const {
        std::vector<std::pair<uint32_t, uint32_t>> overlaps;
        for (auto i{0uz}; i < m_processes.size(); ++i) {
            for (auto j{i + 1}; j < m_processes.size(); ++j) {
                auto [pidA, baseA, boundsA] = m_processes[i];
                auto [pidB, baseB, boundsB] = m_processes[j];
                if (!boundsA || !boundsB)
                    continue;

                if (baseA < baseB + boundsB && baseB < baseA + boundsA)
                    overlaps.emplace_back(pidA, pidB);
            }
        }
        return overlaps;
    }
};
