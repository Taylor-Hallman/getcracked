#include "optional/optional.h"
#include <cassert>
#include <print>

class Foo {
private:
    char mem;
public:
    Foo(int num) : mem(num) {}

    ~Foo() {
        std::println("Calling non-trivial destructor");
    }
};

void fun() {
    aux::optional<Foo> opt;
    assert(!opt.has_value());
    aux::optional<Foo> opt2 = Foo(5);
    assert(opt2.has_value());
    aux::optional<int> opt3(5);
    std::println("{}", opt3.value());
}

int main() {
    fun();
}
