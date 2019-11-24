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
        capacity = 1;

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
        // intended cropping when converting to size_t
        // reserve some headroom to avoid immediate reallocation for arithmetic operators
        // don't use other.capacity since it might be that other.capacity == other.number_blocks already
        capacity = static_cast<size_t>(other.number_blocks * 1.5l);

        number_blocks = other.number_blocks;

        bits_used = other.bits_used;

        storage = new uint32_t[capacity];

        for(size_t i1 = 0; i1 < number_blocks; ++i1)
            storage[i1] = other.storage[i1];
    }
}

// move constructor
aint::aint(aint&& other) noexcept
{
    if(other.number_blocks)
    {
        capacity = other.capacity;

        number_blocks = other.number_blocks;

        storage = other.storage;

        bits_used = other.bits_used;

        // leave the other object in a well defined state
        other.capacity = 0;

        other.number_blocks = 0;

        other.storage = nullptr;

        other.bits_used = 0;
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
    // make use of the move assignment operator who will also free the currently held data
    *this = aint{other};

    return *this;
}

// copy assignment
aint& aint::operator=(const aint& other)
{
    if(this == &other)
        return *this;

    // release owned resources
    delete[] storage;

    // intended cropping when converting to size_t
    // reserve some headroom to avoid immediate reallocation for arithmetic operators
    // don't use other.capacity since it might be that other.capacity == other.number_blocks already
    capacity = static_cast<size_t>(other.number_blocks * 1.5l);

    number_blocks = other.number_blocks;

    bits_used = other.bits_used;

    if(number_blocks)
    {
        storage = new uint32_t[capacity];

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

    capacity = other.capacity;

    number_blocks = other.number_blocks;

    bits_used = other.bits_used;

    storage = other.storage;

    //leave the other object in a well defined state
    other.capacity = 0;

    other.number_blocks = 0;

    other.bits_used = 0;

    other.storage = nullptr;

    return *this;
}

// check if object contains a number
bool aint::zero() const
{
    return !number_blocks;
}

// swaps values of two aints
void aint::swap(aint& other)
{
    aint temp = std::move(other);

    other = std::move(*this);

    *this = std::move(temp);
}

// non-member functions

// output as bits in reverse order (from LSB to MSB)
std::ostream& operator<<(std::ostream& out, const aint& num)
{
    if(num.number_blocks)
    {
        // these are the blocks to be printed fully
        for(size_t i1 = 0; i1 < (num.number_blocks - 1); ++i1)
        {
            size_t bit = 0;

            while(bit < 32)
            {
                (num.storage[i1] & (1<<bit)) ? out << "1" : out << "0";

                ++bit;
            }
        }

        // now only the bits actually used in the last block are printed
        size_t bit = 0;

        while(bit < num.bits_used)
        {
            num.storage[num.number_blocks-1] & (1<<bit) ? out << "1" : out << "0";

            ++bit;
        }
    }
    else
    {
        // a single "0"-bit symbolises that the entire number is equal to zero
        out << "0";
    }

    return out;
}

// input from a stream of 1s and 0s where the order is reversed i.e. LSB to MSB
std::istream& operator>>(std::istream& in, aint& num)
{
    aint temp{0};

    uint32_t block = 0;

    size_t counter = 0;

    // return type for istream.get() is int
    int input = 0;

    while((input = in.get()) != '\n')
    {
        if(input == 49) // the input is char "1"
        {
            block |= (1 << counter);

            ++counter;
        }
        else if(input == 48) //the input is char "0"
            ++counter;
        // ignore all other characters

        // check if the current block is full

        if(counter == 32)
        {
            temp.push_back(block); //TODO: implement function push_back

            block = 0;

            counter = 0;
        }
    }

    temp.bits_used = counter;


}