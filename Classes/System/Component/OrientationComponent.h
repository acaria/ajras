#pragma once
#include "Dir.h"

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