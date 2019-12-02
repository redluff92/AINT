//
// Created by Ralf Burkhardt on 23/11/19.
//

/* Introduction
 *
 * !!! WARNING !!!
 * ! The program works almost perfectly fine with one exception.
 * ! When using std::cin and the corresponding operator>> the syntax
 * ! while(std::cin >> aint) does not work properly.
 * ! The program runs to the end (as it can be seen when looking at the exit code)
 * ! but all io-streams seem to be broken meaning that even simple outputs like
 * ! std::cout << "Hello World" << std::endl; will not be printed anymore.
 * ! The program works fine when the input is NOT ended by the EOF but rather by the loop.
 * !
 * ! This means the following will NOT work:
 * !    aint temp{};
 * !    while(std::cin >> temp){
 * !        //do stuff;
 * !    }
 * !
 * ! Whereas this will work:
 * !    aint temp{};
 * !    for(size_t i1 = 0; i < N; ++i){
 * !        // do stuff
 * !    }
 * !
 * ! I think this has to do with the bit flags but I can't figure out why the input stream breaks the output stream!
 * ! As mentioned above the program finishes with the correct exit code and when inspected in a debugger one can see
 * ! that variables get updated and computed correctly.
 * !
 * ! Again, this is not an error in the arithmetic operators but simply a bug in the io-streams
 * ! so the program can be tested using above described for loops
 *
 *
 * The program works roughly like this:
 *
 * A number consists of multiple "blocks" which are stored in an dynamic array. Each block is of data type uint32_t.
 * The blocks in the array are stored from LSB to MSB but the bits in a block are stored from MSB to LSB.
 * The container therefore has to be read somewhat similar to a hebrew book where you would read from first to last
 * page but on each page you would read from right to left.
 *
 * When reading in input we fill a block until we have read in 32 bits and then we append the block to the container
 * using the function push_back. If the input ends while still filling a block we push_back that block.
 * Since we want to keep track of the exact number of stored bits we keep a counter for the stored blocks
 * in the container and also for the bits actually used in the last block.
 *
 * This design has the advantage that we can easily implement arithmetic operators using the simple school methods
 * of addition, subtraction and multiplication.
 * For division and modulo we use the binary version of the long division algorithm as it is described here:
 * https://en.wikipedia.org/wiki/Division_algorithm#long_division
 *
 * Comparison operators are also largely based on comparing the number of blocks and used bits first and
 * only in extreme cases iterate over the entire array.
 *
 * Accumulative operators are implemented by reusing binary arithmetic operators.
 * Bit shift operators make a simplification by first computing the number of entire blocks that will be added in case
 * of operator<< or cut off in case of operator>>.
 *
 * Memory management is mostly done by the functions reserve() and shrink() or in case of constructors by hand.
 * In general memory management aims to give all aint objects a certain buffer to prevent immediate reallocation after
 * arithmetic operations like for instance operator+=.
 * The function shrink() is intended to free used memory not needed anymore (like after operator-=) while still
 * leaving the number some memory reserve
 *
 *
 */
#include <iostream>
#include "aint.hpp"

// public member functions

// constructors for "small" long numbers
aint::aint(const uint32_t value)
{
    if(value != 0)
    {
        size_t counter = 32;

        // since value != 0 there has to be a MSB
        while( !( value & ( 1 << (counter - 1) ) ) )
            --counter;

        capacity = 1;

        number_blocks = 1;

        storage = new uint32_t[1]{value};

        bits_used = counter;
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
        capacity = (3 * other.number_blocks) / 2 +1;

        number_blocks = other.number_blocks;

        bits_used = other.bits_used;

        storage = new uint32_t[capacity]{0};

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
    // it is the users responsibility to make sure no self assignment is happening

    // release owned resources
    delete[] storage;

    // intended cropping when converting to size_t
    // reserve some headroom to avoid immediate reallocation for arithmetic operators
    // don't use other.capacity since it might be that other.capacity == other.number_blocks already
    capacity = (3 * other.number_blocks) / 2 +1;

    number_blocks = other.number_blocks;

    bits_used = other.bits_used;

    if(number_blocks)
    {
        storage = new uint32_t[capacity]{0};

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
    std::swap(capacity, other.capacity);

    std::swap(number_blocks, other.number_blocks);

    std::swap(storage, other.storage);

    std::swap(bits_used, other.bits_used);
}


// adds the number to the object
aint& aint::operator+=(const aint& b)
{
    *this = (*this + b);

    return *this;
}


// subtracts the number from the object
aint& aint::operator-=(const aint& b)
{
    *this = (*this - b);

    return *this;
}


// multiply the object with the number
aint& aint::operator*=(const aint& b)
{
    *this = (*this * b);

    return *this;
}


// divide the object by the number (integer division)
aint& aint::operator/=(const aint& b)
{
    *this = (*this / b);

    return *this;
}


// divide the object by the number and store the remainder in the object
aint& aint::operator%=(const aint& b)
{
    *this = (*this % b);

    return *this;
}


// shifts the bits in the object by the number from LSB to MSB
aint& aint::operator<<=(size_t shifts)
{
    *this = (*this << shifts);

    return *this;
}


// shifts the bits in the object by the number from MSB to LSB
aint& aint::operator>>=(size_t shifts)
{
    *this = (*this >> shifts);

    return *this;
}


// private memmber functions

void aint::push_back(uint32_t block, size_t counter, bool isvalid)
{
    // isvalid indicates whether counter represents the true number of bits used in block
    if(!isvalid && block)
    {
        counter = 32;

        while(! (block & (1 << (counter - 1) ) ) )
            --counter;
    }
    else if(!isvalid)
        counter = 32;

    bits_used = counter;

    if(number_blocks == capacity)
        reserve((3 * number_blocks) /2 +1);

    storage[number_blocks] = block;

    ++number_blocks;
}


void aint::reserve(size_t new_cap)
{
    // it must be ensured that new_cap >= number_blocks
    // since this functions is private it is the classes responsibility to call it correctly
    if(!new_cap)
        return;

    auto temp_storage = new uint32_t[new_cap]{0};

    for(size_t i1 = 0; i1 < number_blocks; ++i1)
        temp_storage[i1] = storage[i1];

    // release owned resources
    delete[] storage;

    storage = temp_storage;

    capacity = new_cap;
}


void aint::shrink()
{
    // releases parts of the owned resources
    size_t used_blocks = capacity;

    while(used_blocks && !storage[used_blocks - 1])
        --used_blocks;

    // set the object to zero if the entire storage is empty
    if(!used_blocks)
    {
        *this = aint{0};

        return;
    }

    else
    {
        number_blocks = used_blocks;

        size_t bits = 32;

        while( !(storage[number_blocks - 1] & (1<<(bits - 1))))
            --bits;

        bits_used = bits;
    }

    if(capacity > ((3 - number_blocks) / 2 +1))
        reserve((3 * number_blocks) / 2 + 1);
}

// non-member functions

// output in correct bit order
std::ostream& operator<<(std::ostream& out, const aint& num)
{
    if(num.number_blocks)
    {
        for(size_t i1 = num.number_blocks; i1 > 0; --i1)
        {
            size_t bits = 32;

            if(i1 == num.number_blocks)
                bits = num.bits_used;

            while(bits > 0)
            {
                (num.storage[i1-1] & (1<<(bits-1))) ? out << "1" : out << "0";

                --bits;
            }
        }
    }
    else
        out << "0";

    return out;
}
/*
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
}*/


// input from a stream of 1s and 0s where the order is reversed i.e. LSB to MSB
std::istream& operator>>(std::istream& in, aint& num)
{
    // if the users doesn't enter any valid number at all num shall take the value of zero
    // the sequence has to end with "1" otherwise there is no guarantee for the correct functionality of aint

    aint temp{0};

    uint32_t block = 0;

    size_t counter = 0;

    // get rid of leading whitespaces
    while(std::isspace(in.peek()))
        in.get();

    // return type for istream.peek() is int
    int input = in.peek();

    while((input == '0') || (input == '1'))
    {
        if (input == '1')
            block |= (1 << counter);

        ++counter;

        // check if the current block is full
        if (counter == 32)
        {
            temp.push_back(block, counter, true);

            block = 0;

            counter = 0;
        }

        in.get();

        input = in.peek();
    }

    // check if a non binary symbol popped up while still filling a block
    if (block)
        temp.push_back(block, counter, true);

    num = std::move(temp);

    return in;
}


/*
// different solution -> also not working when used like while(std::cin >> aint)
std::istream& operator>>(std::istream& in, aint& num)
{
    aint temp{};

    uint32_t block = 32;

    size_t counter = 0;

    char input = 0;

    while(true)
    {
        if(!in.good())
        {
            in.setstate(std::ios::failbit);

            return in;
        }

        input = static_cast<char>(in.peek());

        if(!std::isspace(input))
            break;

        in.get();
    }

    if(!in.good())
        return in;

    if((input != '0') && (input != '1'))
    {
        in.setstate(std::ios::failbit);

        return in;
    }

    input = static_cast<char>(in.peek());

    while(in.good() && ((input == '0') || (input == '1')))
    {
        if (input == '1')
            block |= (1 << counter);

        ++counter;

        // check if the current block is full
        if (counter == 32)
        {
            temp.push_back(block, counter, true);

            block = 0;

            counter = 0;
        }

        in.get();

        input = static_cast<char>(in.peek());
    }

    // check if terminating symbol popped up while still filling a block
    if (block)
        temp.push_back(block, counter, true);

    num = std::move(temp);

    return in;
}*/


// check for equal values
bool operator==(const aint& a, const aint& b)
{
    // check separately because in this case accessing storage would result in segmentation fault
    if(a.zero() && b.zero())
        return true;

    // if these values differ the numbers can't be the same
    else if( (a.number_blocks != b.number_blocks) || (a.bits_used != b.bits_used))
        return false;

    else
    {
        // at this point a.number_blocks == b.number_blocks
        for(size_t i1 = 0; i1 < a.number_blocks; ++i1)
        {
            if(a.storage[i1] != b.storage[i1])
                return false;
        }

        return true;
    }
}


// check for unequal values by using the operator for equality
bool operator!=(const aint& a, const aint& b)
{
    return !(a==b);
}


// check if the first number is smaller than the second
bool operator<(const aint& a, const aint& b)
{
    // number a can never be less than zero
    if(b.zero())
        return false;

    else if(a.number_blocks < b.number_blocks)
        return true;

    else if(a.number_blocks > b.number_blocks)
        return false;

    else if((a.number_blocks == b.number_blocks) && (a.bits_used < b.bits_used))
        return true;

    else if((a.number_blocks == b.number_blocks) && (a.bits_used > b.bits_used))
        return false;

    else
        // at this point a.number_blocks == b.number_blocks
        for(size_t i1 = a.number_blocks; i1 > 0; --i1)
        {
            if(a.storage[i1-1] < b.storage[i1-1])
                return true;
        }

        return false;
}


// check if the first number is smaller or equal than the second number using corresponding operators
bool operator<=(const aint& a, const aint& b)
{
    return ( (a==b) || (a<b));
}


// check if the first number is larger than the second number using the <= operator
bool operator>(const aint& a, const aint& b)
{
    return !(a<=b);
}


// check if the first number is larger or equal to the second number
bool operator>=(const aint& a, const aint& b)
{
    return !(a<b);
}


// add the numbers together into a new aint object
aint operator+(const aint& a, const aint& b)
{
    if(a.zero())
        return b;

    else if(b.zero())
        return a;

    aint result{};

    // reserve enough memory to store addition result and have some extra space
    result.reserve(a.number_blocks >= b.number_blocks
                   ? ((3 * a.number_blocks) / 2  +1)
                   : ((3 * b.number_blocks) / 2 + 1));

    uint64_t add_res = 0;

    uint64_t overflow = 0;

    for(size_t i1 = 0; i1 < a.number_blocks || i1 < b.number_blocks; ++i1)
    {
        if(i1 < a.number_blocks)
            add_res += a.storage[i1];

        if(i1 < b.number_blocks)
            add_res += b.storage[i1];

        overflow = add_res >>32;

        // intended cropping when casting to uint32_t
        // overflow signals to push_back whether the counter is actually valid
        result.push_back(static_cast<uint32_t>(add_res), 32, static_cast<bool>(overflow));

        add_res = overflow;
    }

    // there could be an overflow left
    if(add_res)
        result.push_back(static_cast<uint32_t>(add_res), 32, false);

    return result;
}


// subtract b from a or return 0 if a <= b
aint operator-(const aint& a, const aint& b)
{
    // instead of negative numbers zero shall be returned
    if(a.zero() || b.zero())
        return a;

    else if(a <= b)
        return aint{};

    // create a copy of a which will later also store the result of
    aint result {a};

    // create the twos complement of b and use the add operator to add it to a
    aint neg{};

    neg.reserve(a.number_blocks + 1);

    // twos complement of b needs to have exactly the same length as a
    for(size_t i1 = 0; i1 < a.number_blocks; ++i1)
    {
        // use negated block from b....
        if(i1 < b.number_blocks)
            neg.push_back(~b.storage[i1], 32, true);

        // ...or use negated empty block to fill up to the same length
        else
            neg.push_back(~static_cast<uint32_t>(0), 32, true);
    }

    // so far only ones complement has been calculated
    // therefore adding 1 will give twos complement
    neg += aint{1};

    // add result and neg together
    uint64_t add_res = 0;

    for(size_t i1 = 0; i1 < result.number_blocks; ++i1)
    {
        add_res += result.storage[i1];

        add_res += neg.storage[i1];

        result.storage[i1] = static_cast<uint32_t>(add_res);

        add_res >>=32;
    }

    // there will be some overflow left in add_res in the end which must be set to zero for shrink() to work
    for(size_t i1 = result.number_blocks; i1 < result.capacity; ++i1)
        result.storage[i1] = 0;

    result.shrink();

    return result;
}


// multiply two numbers together
aint operator*(const aint& a, const aint& b)
{
    if(a.zero() || b.zero())
        return aint{0};

    aint result{};

    // reserve enough memory to store multiplication result and have some extra space
    result.reserve((3 * (a.number_blocks + b.number_blocks)) / 2 +1);

    uint64_t mult_res = 0;

    for(size_t i1 = 0; i1 < b.number_blocks; ++i1)
    {
        for(size_t i2 = 0; i2 < a.number_blocks; ++i2)
        {
            mult_res = static_cast<uint64_t>(a.storage[i2]) * static_cast<uint64_t>(b.storage[i1]);

            // write the potential overflow to the correct position in result
            for(size_t i3 = i1 + i2; mult_res > 0; ++i3)
            {
                mult_res += result.storage[i3];

                // intended cropping when converting to uint32_t
                result.storage[i3] = static_cast<uint32_t>(mult_res);

                mult_res >>= 32;
            }
        }
    }
    // adjust number_blocks and bits_used for result without calling shrink() since this might move all the values

    size_t used_blocks = a.number_blocks + b.number_blocks;

    while(!result.storage[used_blocks -1])
        --used_blocks;

    result.number_blocks = used_blocks;

    size_t bits = 32;

    while(!(result.storage[result.number_blocks -1] & (1 <<(bits-1))))
        --bits;

    result.bits_used = bits;

    return result;
}


// divide the first number by the second number (integer division)
aint operator/(const aint& a, const aint& b)
{
    // division by zero will return zero
    if (b.zero() || (a < b))
        return aint{};

    if(b == aint{1})
        return a;

    aint quotient{};

    // since the quotient can never be larger than the original number memory reservation can be done more conservative
    quotient.reserve(a.number_blocks + 1);

    aint remainder{};

    for(size_t i1 = a.number_blocks; i1 > 0; --i1)
    {
        size_t used_bits = (i1 == a.number_blocks)
                            ? a.bits_used
                            : 32;

        for(size_t i2 = used_bits; i2 > 0; --i2)
        {
            uint32_t  bit = (a.storage[i1-1] & (1 << (i2-1)));

            if(!remainder.zero())
                remainder <<= 1;

            if(remainder.zero() && bit)
                remainder = aint{1};

            else if(bit)
                remainder.storage[0] |= 1;

            if(remainder >= b)
            {
                remainder -= b;

                quotient.storage[i1-1] |= (1 << (i2-1));
            }
        }

    }

    // quotient might be very small and requires less memory
    quotient.shrink();

    return quotient;
}


//  return the remainder of dividing the first number by the second number
aint operator%(const aint& a, const aint& b)
{
    // modulo by zero will return the original number
    if (b.zero() || (a < b))
        return a;

    if((b == aint{1}) || (a == b))
        return aint{};

    aint quotient{};

    // since the quotient can never be larger than the original number memory reservation can be done more conservative
    quotient.reserve(a.number_blocks + 1);

    aint remainder{};

    for(size_t i1 = a.number_blocks; i1 > 0; --i1)
    {
        size_t used_bits = (i1 == a.number_blocks)
                            ? a.bits_used
                            : 32;

        for(size_t i2 = used_bits; i2 > 0; --i2)
        {
            uint32_t  bit = (a.storage[i1-1] & (1 << (i2-1)));

            if(!remainder.zero())
                remainder <<= 1;

            if(remainder.zero() && bit)
                remainder = aint{1};

            else if(bit)
                remainder.storage[0] |= 1;

            if(remainder >= b)
            {
                remainder -= b;

                quotient.storage[i1-1] |= (1 << (i2-1));
            }
        }

    }

    // remainder might be very small and requires less memory
    remainder.shrink();

    return remainder;
}


// shift bits from LSB to MSB
aint operator<<(const aint& num, size_t shifts)
{
    if(!shifts || num.zero())
        return num;

    aint result{};

    // number of additional empty blocks created by the shift. Those blocks represent the LSBs of the number
    size_t add_blocks = shifts / 32;

    shifts %= 32;

    // reserve memory for the result and additional space
    // since numbers can use up space for additional blocks very quickly when shifting
    // reservation of memory is limited to 50 additional blocks as a buffer
    result.reserve(((3 * (num.number_blocks + add_blocks)) / 2 +1)
                    < (num.number_blocks + add_blocks + 50)
                    ? ((3 * (num.number_blocks + add_blocks)) / 2 +1)
                    : (num.number_blocks + add_blocks + 50));

    size_t counter_shifts = (32 - shifts);

    for (size_t i1 = num.number_blocks; i1 > 0; --i1)
    {
        if(shifts)
            result.storage[i1 + add_blocks] |= (num.storage[i1 - 1] >> counter_shifts);

        result.storage[i1 + add_blocks - 1] = (num.storage[i1 - 1] << shifts);
    }

    // set the correct values for result

    result.number_blocks = add_blocks + num.number_blocks;

    if(result.storage[result.number_blocks])
    {
        ++result.number_blocks;

        result.bits_used = num.bits_used + shifts - 32;
    }

    else
        result.bits_used = num.bits_used + shifts;

    return result;
}


// shift bits from MSB to LSB
aint operator>>(const aint& num, size_t shifts)
{
    if(!shifts || num.zero())
        return num;

    // number of blocks that will be cut off entirely by the shift
    size_t cut_blocks = shifts / 32;

    shifts %= 32;

    aint result{};

    // check if the entire number will be cut off
    if(cut_blocks >= num.number_blocks || ((cut_blocks +1 == num.number_blocks) && (num.bits_used <= shifts)))
        return result;

    // reserve memory for the result and additional space
    result.reserve(((3 * (num.number_blocks - cut_blocks)) /2 +1));

    size_t counter_shifts = (32 - shifts);

    for(size_t i1 = 0; i1 < (num.number_blocks - cut_blocks); ++i1)
    {
        result.storage[i1] = (num.storage[i1 + cut_blocks] >> shifts);

        if(i1 && shifts)
            result.storage[i1 -1] |= (num.storage[i1 + cut_blocks] << counter_shifts);
    }

    result.number_blocks = num.number_blocks - cut_blocks;

    if(!result.storage[result.number_blocks - 1])
    {
        --result.number_blocks;

        result.bits_used = num.bits_used + 32 - shifts;
    }

    else
        result.bits_used = num.bits_used - shifts;

    return result;
}
