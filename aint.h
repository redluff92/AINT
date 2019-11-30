//
// Created by Ralf Burkhardt on 23/11/19.
//

#ifndef AINT_AINT_H
#define AINT_AINT_H


#include <stdint-gcc.h>
#include <glob.h>
#include <iostream>

class aint final
{
public:

    explicit aint(const uint32_t = 0);

    aint(const aint&);

    aint(aint&& other) noexcept;

    ~aint();

    aint& operator=(uint32_t);

    aint& operator=(const aint&);

    aint& operator=(aint&&) noexcept;

    bool zero() const;

    void swap(aint&);

    // accumulative operators
    aint& operator+=(const aint&);

    aint& operator-=(const aint&);

    aint&operator*=(const aint&);

    aint& operator/=(const aint&);

    aint&operator%=(const aint&);

    aint& operator<<=(size_t);

    aint& operator>>=(size_t);

    // non-member functions

    // I/O operators
    friend std::ostream& operator<<(std::ostream&,const aint&);

    friend std::istream& operator>>(std::istream&, aint&);

    // comparison operators
    friend bool operator==(const aint&, const aint&);

    friend bool operator!=(const aint&, const aint&);

    friend bool operator<(const aint&, const aint&);

    friend bool operator<=(const aint&, const aint&);

    friend bool operator>(const aint&, const aint&);

    friend bool operator>=(const aint&, const aint&);

    // binary arithmetic operators
    friend aint operator+(const aint&, const aint&);

    friend aint operator-(const aint&, const aint&);

    friend aint operator*(const aint&, const aint&);

    friend aint operator/(const aint&, const aint&);

    friend aint operator%(const aint&, const aint&);

    friend aint operator<<(const aint&, size_t);

    friend aint operator>>(const aint&, size_t);

private:

    // reserved storage
    size_t capacity = 0;

    // actual size of the array i.e. number of used uint32_ts
    size_t number_blocks = 0;

    // array containing the number
    // the least significant block is at position [0] but the bits within a specific block are ordered from MSB to LSB
    uint32_t* storage = nullptr;

    // keep track of how many bits in the last i.e. most significant block are actually used
    size_t bits_used = 0;

    // functions for memory management
    void push_back(uint32_t, size_t = 0, bool = false);

    void reserve(size_t);

    void shrink();
};

#endif //AINT_AINT_H
