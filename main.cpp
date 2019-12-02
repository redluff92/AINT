#include <list>
#include <iostream>
#include <utility>
#include "aint.hpp"


aint gcd(aint a, aint b) {
    if(a<b) a.swap(b);
    size_t iteration = 0;
    for(;;) {
        std::cout << "Called loop at iteration: " << iteration++ << " with b: " << b << std::endl;
        if(b.zero()) return a;
        a%=b;
        a.swap(b);
    }
}
/*
int main() {
    std::list<aint> l;
    aint tmp;
    //while(std::cin >> tmp) l.emplace_back(std::move(tmp));
    for(size_t i1 = 0; i1 < 2; ++i1)
    {
        std::cin >> tmp;
        l.emplace_back((std::move(tmp)));
    }
    if(l.empty()) return 1;

    tmp=std::move(l.front());
    l.pop_front();
    for(auto&n:l) tmp = gcd(tmp, n);

    std::cout << tmp << std::endl;

    return 0;
}*/

int main()
{
    aint number1{}, number2{0};

    std::cin >> number1;

    std::cout << number1 << std::endl;

    //std::cin >> number2;

    //std::cout << number2 << std::endl;

    std::cout << (number1 * aint{7}) << std::endl;

    return 0;
}
