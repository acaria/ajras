#pragma once

#include "CoreLib.h"

struct TrailComponent
{
    void set(unsigned memoryLength)
    {
        this->memoryLength = memoryLength;
    }
    
    //input
    unsigned memoryLength = 5;
    
    //output
    std::list<lib::v2u> tail;
};