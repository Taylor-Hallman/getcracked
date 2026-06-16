#include <vector>
#include <limits>

struct Bucket {
    int Start, End, Count;
};

using Buckets = std::vector<Bucket>;  

class Histogram { 
public: 
    Histogram(size_t count, size_t size) { 
        m_buckets.resize(count + 1);
        int start = 0;
        for (size_t i{}; i < count; ++i) {
            m_buckets[i].Start = start;
            m_buckets[i].End = start + size;
            m_buckets[i].Count = 0;
            start += size;
        }
        m_buckets[count].Start = start;
        m_buckets[count].End = std::numeric_limits<int>::max(); 
        m_buckets[count].Count = 0;

        m_shift_amt = std::countr_zero(size);
    }
    Buckets Build(const std::vector<int>& latencies) {
        for (const int& latency : latencies) {
            if (latency < 0)
                continue;
            size_t idx = std::min((static_cast<size_t>(latency) >> m_shift_amt), (m_buckets.size() - 1));
            m_buckets[idx].Count++;
        }
        return m_buckets;
    }    
private:
    Buckets m_buckets;
    size_t m_shift_amt;
};
