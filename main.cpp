#include "lfspscq/LockFreeQueue.h"
#include "util/Point.h"

#include <iostream>

int main() {
    getcracked::SPSCQ<Point> q(8);
    std::cout << q.size() << std::endl; // 0
    Point p(1, 2);
    Point p2;
    q.push(p);
    q.push(p2);
    std::cout << q.size() << std::endl; // 2
    Point p3;
    q.pop(p3);
    p3.print(); // 1, 2
    std::cout << q.size() << std::endl; // 1
} // Delete
