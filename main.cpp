#include "LRUCache/lru_cache.h"
#include <string>
#include <cassert>

int main() {
    LRUCache<std::string, double> cache(3);

    // Insert some instrument prices
    cache.put("AAPL", 185.50);
    cache.put("MSFT", 378.20);
    cache.put("GOOG", 141.75);

    // Retrieve a value (marks it as most recently used)
    std::optional<double> apple = cache.get("AAPL"); // 185.50
    assert(apple.has_value());
    assert(apple.value() == 185.5);

    // Check if a key exists (does NOT affect LRU order)
    bool has_meta = cache.contains("META"); // false
    assert(!has_meta);

    // Cache is full (3/3). Inserting a new key evicts the LRU item.
    // MSFT was least recently used (GOOG was accessed via get, AAPL via get).
    cache.put("META", 505.75);
    std::optional<double> msft = cache.get("MSFT"); // std::nullopt (evicted)
    assert(!msft.has_value());

    // Update an existing key
    cache.put("AAPL", 190.00);
    std::optional<double> updated = cache.get("AAPL"); // 190.00
    assert(updated.has_value());
    assert(updated.value() == 190.0);

    // Size and capacity
    std::size_t count = cache.size();     // 3
    std::size_t cap = cache.capacity();   // 3
    assert(count == 3);
    assert(cap == 3);

    // Remove a key
    bool removed = cache.erase("GOOG");   // true
    bool again = cache.erase("GOOG");     // false (already removed)
    std::size_t after = cache.size();     // 2
    assert(removed);
    assert(!again);
    assert(after == 2);
}
