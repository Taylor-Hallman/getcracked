#include <new>
#include <vector>
#include <cstdint>
#include <chrono>

constexpr auto CacheLineSize = std::hardware_destructive_interference_size;

struct alignas(CacheLineSize) CacheLinePadded {
    uint8_t m_data = 0;
};

double MeasureL1ColdReadLatency() {
    constexpr auto CacheFloodSize = CacheLineSize * (2 << 12); // 500k bytes 
    constexpr auto Trials = 20'000;
    CacheLinePadded data;
    std::vector<uint8_t> Vec(CacheFloodSize);
    double FloodTime = 0.0;
    volatile uint64_t sink = 0;

    using namespace std::chrono;
    auto TimeStart = steady_clock::now();

    for (size_t trials{}; trials < Trials; ++trials) {
        auto FloodStart = steady_clock::now();
        for (size_t i{}; i < CacheFloodSize; i += CacheLineSize) {
            sink += Vec[i];
        }
        auto FloodEnd = steady_clock::now();
        sink += data.m_data;
        FloodTime += static_cast<double>(duration_cast<nanoseconds>(FloodEnd - FloodStart).count());
    }

    auto TimeEnd = steady_clock::now();
    double TotalTime = static_cast<double>(duration_cast<nanoseconds>(TimeEnd - TimeStart).count());
    return (TotalTime - FloodTime) / Trials;
}
