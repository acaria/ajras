#pragma once
#include "Headers.h"

struct OrientationComponent
{
    OrientationComponent()
    {
        this->curDir = Dir::kDown;
        this->lastDir = Dir::kDown;
    }
    
    unsigned curDir;
    unsigned lastDir;
};