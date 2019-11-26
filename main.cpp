#include <iostream>
#include "aint.h"

int main()
{
    aint first{0};

    aint second{0};

    std::cin >> first;

    std::cin >> second;

    std::cout << (first - second) << std::endl;

    return 0;
}