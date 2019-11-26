//
// Created by Ralf Burkhardt on 23/11/19.
//

#include <iostream>
#include "aint.h"


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

        storage = new uint32_t{value};

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
        capacity = static_cast<size_t>(other.number_blocks * 1.5l) + 1;

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
    capacity = static_cast<size_t>(other.number_blocks * 1.5l) + 1;

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
    aint temp = std::move(other);

    other = std::move(*this);

    *this = std::move(temp);
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
        reserve(static_cast<size_t>(number_blocks * 1.5l) + 1);

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

    if(capacity > (number_blocks * 1.5l + 1))
        reserve(static_cast<size_t>(number_blocks * 1.5l) +1);

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
    // if the users doesn't enter any valid number at all num shall take the value of zero
    // the sequence has to end with "1" otherwise there is no guarantee for the correct functionality of aint
    aint temp{0};

    uint32_t block = 0;

    size_t counter = 0;

    // return type for istream.get() is int
    int input = 0;

    while((input = in.get()) != '\n')
    {
        if(input == '1')
        {
            block |= (1 << counter);

            ++counter;
        }
        else if(input == '0')
            ++counter;
        // ignore all other characters

        // check if the current block is full
        if(counter == 32)
        {
            temp.push_back(block, counter, true);

            block = 0;

            counter = 0;
        }
    }

    // check if '\n' popped up while still filling a block
    if(block)
        temp.push_back(block, counter, true);

    num = std::move(temp);

    return in;
}


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

    else if( (a.number_blocks == b.number_blocks) && (a.bits_used < b.bits_used))
        return true;

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
    result.reserve( a.number_blocks >= b.number_blocks
                    ? static_cast<size_t>(a.number_blocks * 1.5l) + 1
                    : static_cast<size_t>(b.number_blocks * 1.5l) + 1);

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

    //TODO: use += operator once implemented
    neg = neg + aint{1};

    // add result and neg together and
    uint64_t add_res = 0;

    for(size_t i1 = 0; i1 < result.number_blocks; ++i1)
    {
        add_res += result.storage[i1];

        add_res += neg.storage[i1];

        result.storage[i1] = static_cast<uint32_t>(add_res);

        add_res >>=32;
    }

    // there will be some overflow left in add_res in the end which is supposed to be ignored

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
    result.reserve(static_cast<size_t>((a.number_blocks + b.number_blocks) * 1.5l) +1);

    uint64_t mult_res = 0;

    for(size_t i1 = 0; i1 < b.number_blocks; ++i1)
    {
        for(size_t i2 = 0; i2 < a.number_blocks; ++i2)
        {
            mult_res = a.storage[i2] * b.storage[i1];

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


//TODO: implementation of division and modulo

// shift bits from LSB to MSB
aint operator<<(const aint& num, size_t shifts)
{
    if(!shifts)
        return num;

    else if(num.zero())
        return aint{0};

    aint result{};

    // number of additional empty blocks created by the shift
    size_t add_blocks = shifts / 32;

    shifts %= 32;

    // reserve memory for the result and additional space
    // since numbers can use up space for additional blocks very quickly when shifting
    // reservation of memory is limited to 50 additional blocks as a reserve
    result.reserve(static_cast<size_t>((num.number_blocks + add_blocks) * 1.5l) +1
                   < (num.number_blocks + add_blocks + 50)
                   ? static_cast<size_t>((num.number_blocks + add_blocks) * 1.5l) +1
                   : (num.number_blocks + add_blocks + 50));


    size_t counter_shifts = (32 - shifts);

    for(size_t i1 = num.number_blocks; i1 > 0; --i1)
    {
        result.storage[i1 + add_blocks] |= (num.storage[i1 -1] >> counter_shifts);

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

}

