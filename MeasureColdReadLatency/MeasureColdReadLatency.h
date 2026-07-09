#include <chrono>
#include <thread>
#include <cassert>
#include <immintrin.h>

static constexpr size_t num_trials{ 100uz };
static constexpr int64_t calibration_window{ 100 };

double calibrate() {
    auto cycles_start{ __rdtsc() };
    auto time_start{ std::chrono::high_resolution_clock::now() };

    std::this_thread::sleep_for(std::chrono::milliseconds(calibration_window));

    auto cycles_end{ __rdtsc() };
    auto time_end{ std::chrono::high_resolution_clock::now() };

    auto elapsed_ns{ static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(time_end - time_start ).count()) };
    return static_cast<double>(cycles_end - cycles_start) / elapsed_ns;
}

template<typename RandomAccessItr>
double median(RandomAccessItr begin, RandomAccessItr end) {
    auto size{ std::distance(begin, end) };
    auto mid{ size / 2 };
    assert(size > 0);

    std::nth_element(begin, begin + mid, end);
    auto upper{ *(begin + mid) };
    if (size % 2)
        return upper;
    
    std::nth_element(begin, begin + mid - 1, end);
    return (upper + *(begin + mid - 1)) / 2.0;
}

double MeasureL1ColdReadLatency() {
    std::vector<double> measurements(num_trials);

    double cycles_per_ns{ calibrate() };

    const int* ptr = new int(0);

    for (size_t i{}; i < num_trials; ++i) {
        _mm_clflush(static_cast<const void*>(ptr)); // flush the cache line
        _mm_mfence(); // wait for the flush to finish

        _mm_lfence();
        auto cycles_start{ __rdtsc() };
        
        // perform the cold read
        [[maybe_unused]] int val = *(volatile int*)ptr;

        _mm_lfence();
        auto cycles_end{ __rdtsc() };

        auto cycles{ cycles_end - cycles_start };
        measurements[i] = static_cast<double>(cycles) / cycles_per_ns;
    }

    delete ptr;

    // return the median rather than the mean so extremes don't throw off the measurement
    return median(measurements.begin(), measurements.end());
}
