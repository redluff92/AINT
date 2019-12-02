#include <list>
#include <iostream>
#include <utility>
#include "aint.hpp"


aint gcd(aint a, aint b) {
    if(a<b) a.swap(b);
    for(;;) {
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
    aint number1{};

    aint number2{};

    std::cin >> number1;

    std::cin >> number2;

    std::cout << gcd(number1, number2) << std::endl;

    return 0;
}