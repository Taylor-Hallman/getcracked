#include "event_stream/event_stream.h"

#include <iomanip>
#include <iostream>

int main() {
    Samples samples{
        { 2.4, { "main" } },
            { 2.5, { "main", "foo", "bar", } },
            { 2.6, { "main", "bar", "foo",  } },
            { 3.0, { "main", } },
    };
    const auto events = GenerateEvents(samples);

    for (auto& event : events) {
        std::cout << std::fixed << std::setprecision(2) << event.Elapsed << " " << event.Method << "\n";
    }
}
