#include <iostream>
#include "aint.h"

int main()
{
    aint a{};

    aint b{};

    std::cin >> a;

    std::cin >> b;

    std::cout << (a / b) << std::endl;

    return 0;
}