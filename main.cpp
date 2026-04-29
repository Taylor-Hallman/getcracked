#include "write_supremacy_concurrent/InstantWriteMultipleRead.h"
#include <iostream>
#include <thread>
#include <memory>

using namespace getcracked;

void write_func(std::shared_ptr<InstantWriteMultipleRead<int>> q, int val) {
    q->Write(val);
}

void read_func(std::shared_ptr<InstantWriteMultipleRead<int>> q) {
    int result;
    if (q->Read(result))
        std::cout << result;
    else
        std::cout << "Could not read value: Queue was empty";
}

int main() {
    std::shared_ptr<InstantWriteMultipleRead<int>> q = std::make_shared<InstantWriteMultipleRead<int>>();
    //std::thread write_thread1(write_func, q, 1);
    //write_thread1.join();
    std::thread write_thread2(write_func, q, 2);
    std::thread read_thread1(read_func, q);
    std::thread read_thread2(read_func, q);
    write_thread2.join();
    read_thread1.join();
    read_thread2.join();
    std::cout << "\n";
}
