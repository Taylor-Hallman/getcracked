#pragma once

#include <iostream>

// Simple struct to help test data structures

struct Point {
    int x, y;

    Point() : x(0), y(0) {
        std::cout << "Create\n";
    }
    Point(int x, int y) : x(x), y(y) {
        std::cout << "Create\n";
    }
    Point(const Point& other) : x(other.x), y(other.y) {
        std::cout << "Copy\n";
    }
    Point& operator=(const Point& other) {
        x = other.x;
        y = other.y;
        std::cout << "Copy\n";
        return *this;
    }
    Point(Point&& other) noexcept : x(other.x), y(other.y) {
        std::cout << "Move\n";
    }
    Point& operator=(Point&& other) noexcept {
        x = other.x;
        y = other.y;
        std::cout << "Move\n";
        return *this;
    }
    ~Point() {
        std::cout << "Delete\n";
    }

    void print() {
        std::cout << "(" << x << ", " << y << ")\n";
    }
};
