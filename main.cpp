#include <iostream>
#include "aint.h"

int main()
{
    aint first{}, second{};

    std::cin >> first;

    std::cin >> second;

    std::cout << (first * second) << std::endl;

    return 0;
}