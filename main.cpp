#include <iostream>
#include "aint.h"

int main()
{
    std::cout << (aint{0} < aint{210}) << std::endl;

    return 0;
}