#include "MeasureColdReadLatency/MeasureColdReadLatency.h"
#include <iostream>

int main() {
    double d = MeasureL1ColdReadLatency();
    std::cout << d << "\n";
}
