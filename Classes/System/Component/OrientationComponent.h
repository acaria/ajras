#pragma once
#include "Headers.h"

struct OrientationComponent
{
    OrientationComponent()
    {
        this->curDir = Dir::Down;
        this->lastDir = Dir::Down;
    }
    
    Dir curDir;
    Dir lastDir;
};