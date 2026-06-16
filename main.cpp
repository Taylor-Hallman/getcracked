#include "histogram/histogram.h"

int main() {
    Histogram h(4, 8);
    std::vector<int> latencies{ 3, 5, 7, 2, 9 };
    Buckets b = h.Build(latencies);
    return 0;
}
