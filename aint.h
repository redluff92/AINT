//
// Created by Ralf Burkhardt on 23/11/19.
//

#ifndef AINT_AINT_H
#define AINT_AINT_H


#include <stdint-gcc.h>
#include <glob.h>

class aint final
{
public:

    explicit aint(uint32_t = 0);

    aint(const aint&);

    ~aint();

private:

    uint32_t* storage = nullptr;

    size_t number_blocks = 0;

    size_t bits_used = 0;
};
#endif //AINT_AINT_H
