#include <chrono>
#include <ctime>
#include <iostream>

struct Timer {
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    std::chrono::duration<float, std::ratio<1, 1000>> duration;

    Timer() {
        start = std::chrono::high_resolution_clock::now();
    }

    ~Timer() {
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        //std::cout << duration.count() << std::endl;
    }

    float getDuration() { 
        duration = std::chrono::high_resolution_clock::now() - start;
        return duration.count();
    }
};
