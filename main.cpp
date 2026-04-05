#include "mutex/Mutex.h"
#include "timer/Timer.h"
#include <thread>
#include <cassert>
#include <iostream>

int main() {
    float avg = 0.0f;
    for (int i = 0; i < 1000; i++) {
        Timer timer;
        Mutex mutex;
        const auto count = 10000;
        auto counter = 0;
        auto worker = [&] {
            for (int i = 0; i < count; i++) {
                mutex.lock();
                counter++;
                mutex.unlock();
            }
        };
        std::thread threadA(worker), threadB(worker);
        threadA.join();
        threadB.join();
        avg += timer.getDuration();
    }
    avg /= 1000.0f;
    std::cout << avg << std::endl;
}
