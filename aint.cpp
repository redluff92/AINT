//
// Created by Ralf Burkhardt on 23/11/19.
//

#include <iostream>
#include "aint.h"

// constructors for "small" long numbers
aint::aint(const uint32_t value)
{
    if(value != 0)
    {
        number_blocks = 1;

        storage = new uint32_t{value};

        // count the number of bits used i.e. find the MSB of value
        // since value != 0 we have guarantee to find a used bit
        bits_used = 32;

        while( !( value & ( 1 << (bits_used - 1) ) ) )
            --bits_used;

    }

}

// copy constructor
aint::aint(const aint& other)
{
    if(other.number_blocks)
    {
        number_blocks = other.number_blocks;

        bits_used = other.bits_used;

        storage = new uint32_t[number_blocks];

        for(size_t i1 = 0; i1 < number_blocks; ++i1)
            storage[i1] = other.storage[i1];
    }
}

// move coy constructor
aint::aint(aint&& other) noexcept
{
    if(other.number_blocks)
    {
        number_blocks = other.number_blocks;

        bits_used = other.bits_used;

        storage = other.storage;

        // leave the other object in a well defined state
        other.number_blocks = 0;

        other.bits_used = 0;

        other.storage = nullptr;
    }
}

// destructor does not to be virtual since inheritance is disabled
aint::~aint()
{
    delete[] storage;
}

// copy assignment from uint32_t
aint& aint::operator=(const uint32_t other)
{
    // make use of the move assignment operator
    *this = aint(other);

    return *this;
}

aint& aint::operator=(const aint& other)
{
    if(this == &other)
        return *this;

    // release owned resources
    delete[] storage;

    number_blocks = other.number_blocks;

    bits_used = other.bits_used;

    if(number_blocks)
    {
        storage = new uint32_t[number_blocks];

        for (size_t i1 = 0; i1 < number_blocks; ++i1)
            storage[i1] = other.storage[i1];
    }

    return *this;
}

// move assignment
aint& aint::operator=(aint&& other) noexcept
{
    // check against self assignment via std::move() is the user's responsibility

    // release owned resources
    delete[] storage;

    number_blocks = other.number_blocks;

    bits_used = other.bits_used;

    storage = other.storage;

    //leave the other object in a well defined state
    other.number_blocks = 0;

    other.bits_used = 0;

    other.storage = nullptr;

    return *this;
}

// check if object contains a number
bool aint::zero() const
{
    return !storage;
}

// swaps values of two aints
void aint::swap(aint& other)
{
    aint temp = std::move(other);

    other = std::move(*this);

    *this = std::move(temp);
}