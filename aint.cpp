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
    number_blocks = other.number_blocks;

    bits_used = other.bits_used;

    if(number_blocks)
    {
        storage = new uint32_t[number_blocks];

        for(size_t i1 = 0; i1 < number_blocks; ++i1)
            storage[i1] = other.storage[i1];
    }
}

// destructor
aint::~aint()
{
    delete[] storage;
}