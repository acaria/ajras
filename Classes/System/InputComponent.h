#pragma once
#include "Headers.h"

struct InputComponent
{
    unsigned orientation;
    unsigned lastOrientation;
    
    std::list<std::function<bool(unsigned)>> predicates;
};