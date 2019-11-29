#include <iostream>
#include "aint.h"

int main()
{
    aint test{};

    std::cin >> test;

    std::cout << (test >> 75) << std::endl;

    return 0;
}