#include <iostream>
#include "shared_ptr/shared_ptr.h"
int main() {
    int* ptr = new int(1);
    getcracked::shared_ptr<int> sh_ptr(ptr);
    std::cout << sh_ptr.get_count() << std::endl;
    getcracked::shared_ptr<int> sh_ptr2(std::move(sh_ptr));
    std::cout << sh_ptr.get_count() << " " << sh_ptr2.get_count() << std::endl;
}
