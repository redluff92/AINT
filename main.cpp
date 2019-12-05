#include <list>
#include <iostream>
#include <utility>
#include "aint.hpp"

aint gcd(aint a, aint b) {
    if(a<b)
        a.swap(b);
    int i1 = 0;
    for(;i1 < 250;++i1) {

        if(b.zero()) return a;
        a%=b;
        a.swap(b);
    }
}

int main() {
    std::list<aint> l;
    aint tmp;
    //while(std::cin >> tmp) l.emplace_back(std::move(tmp));
    for(int i1 = 0; i1 < 2; ++i1)
    {
        std::cin >> tmp;

        l.emplace_back(std::move(tmp));
    }

    if(l.empty()) return 1;

    tmp=std::move(l.front());
    l.pop_front();
    for(auto&n:l) {
        std::cout << "doing " << tmp << std::endl;
        tmp = gcd(tmp, n);
    }
    std::cout << tmp << std::endl;
    return 0;
}

/*
int main()
{
    aint a{};

    std::cin >> a;

    std::cout << a << std::endl;

    aint b{};

    std::cin >> b;

    std::cout << b << std::endl;

    std::cout << (a % b) << std::endl;
} */
