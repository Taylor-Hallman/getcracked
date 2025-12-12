#include "/shared_ptr/shared_ptr.h"
#include "/util/Point.h"

#include <iostream>

int main() {
    getcracked::shared_ptr<Point> p(new Point(1, 2)); // Create
    std::cout << p.get_count() << std::endl; // 1
    {
        getcracked::shared_ptr<Point> p2 = p;
        std::cout << p.get_count() << std::endl; // 2
    }
    std::cout << p.get_count() << std::endl; // 1
    getcracked::shared_ptr<Point> p3(std::move(p)); // Move
    std::cout << p.get_count() << std::endl; // 0
    std::cout << p3.get_count() << std::endl; // 1
    p.reset(new Point(3, 4)); // Delete Create
    p3 = p;
    std::cout << p.get_count() << std::endl; // 2
    std::cout << p3.get_count() << std::endl; // 2
    getcracked::shared_ptr<Point> p4(new Point(5, 6)); // Create
    p4 = std::move(p3); // Delete Move
    std::cout << p.get_count() << std::endl; // 2
    std::cout << p3.get_count() << std::endl; // 0
    std::cout << p4.get_count() << std::endl; // 2
} // Delete