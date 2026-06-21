#pragma once

#include <algorithm>
#include <stack>
#include <vector>
#include <string>

enum class Kind {
      Begin,
      End,
};

struct Event {
      Kind Type;
      double Elapsed;
      std::string Method;
};

using Events = std::vector<Event>;

struct Sample {
      double Elapsed;
      std::vector<std::string> Stack;
};

using Samples = std::vector<Sample>;

Events GenerateEvents(const Samples& samples) {
    size_t size = samples.size();
    if (!size)
        return {};
    Events events;
    const Sample* prev = &samples[0];
    for (auto& method : prev->Stack)
        events.emplace_back(Kind::Begin, prev->Elapsed, method);
    for (auto itr = samples.begin() + 1; itr != samples.end(); ++itr) {
        auto prevSize{std::ssize(prev->Stack)};
        auto thisSize{std::ssize(itr->Stack)};
        auto changeItr = std::mismatch(prev->Stack.begin(), prev->Stack.end(), itr->Stack.begin(), itr->Stack.end()).first;
        ptrdiff_t changeIdx = std::distance(prev->Stack.begin(), changeItr);
        for (auto i{prevSize - 1}; i >= changeIdx; --i)
            events.emplace_back(Kind::End, itr->Elapsed, prev->Stack[i]);
        for (auto i{changeIdx}; i < thisSize; ++i)
            events.emplace_back(Kind::Begin, itr->Elapsed, itr->Stack[i]);
        prev = &*itr;
    }
    return events;
}
