#include "latestQueue/latest_queue.h"
#include <cassert>

int main() {
    getcrackedQueue<Instrument> instruments;
    instruments.write({ "GOOG", 94.5 });
    instruments.write({ "APPL", 200 });
    instruments.write({ "GOOG", 95 });
    const auto size = instruments.size(); // 2
    const auto initialRead = instruments.read(); // GOOG 95
    const auto nextRead = instruments.read(); // APPL 200
    const auto isEmpty = instruments.empty(); // true
                                              
    assert(size == 2);
    assert(initialRead.has_value());
    auto initialReadValue = initialRead.value();
    assert(initialReadValue.Symbol == "GOOG");
    assert(initialReadValue.Price == 95);
    assert(nextRead.has_value());
    auto nextReadValue = nextRead.value();
    assert(nextReadValue.Symbol == "APPL");
    assert(nextReadValue.Price == 200);
    assert(isEmpty);
}
