#include "stop_token/StopToken.h"
#include <stop_token>
#include "timer/Timer.h"
#include <thread>
#include <cassert>
#include <iostream>

//using namespace getcracked;

class Worker {
public:
    void run(std::stop_token stopToken) {
        while (!stopToken.stop_requested()) {
            std::cout << "Working..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        std::cout << "Cancelled" << std::endl;
    }
};

int main() {
    std::stop_source stopSource;
    std::stop_token stopToken = stopSource.get_token();

    Worker worker;
    std::thread t(&Worker::run, &worker, stopToken);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    stopSource.request_stop();

    t.join();

    return 0;
}
