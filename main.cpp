#include "BaseAndBounds/BaseAndBounds.h"
#include <print>

int main() {
    RegionChecker rc;
    rc.add_process(0, 0, 300);
    rc.add_process(1, 200, 300);
    rc.add_process(2, 400, 300);
    rc.add_process(3, 700, 100);
    
    auto overlaps{ rc.find_overlaps() };
    for (auto [pid1, pid2] : overlaps) {
        std::println("PID {0} and PID {1} overlap", pid1, pid2);
    }
}
