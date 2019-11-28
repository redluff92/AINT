#include <iostream>
#include "aint.h"

int main()
{
    aint test{};

    std::cin >> test;

    std::cout << (test << 65) << std::endl;

    return 0;
}