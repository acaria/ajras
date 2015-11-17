#pragma once
#include "Dir.h"

struct OrientationComponent
{
    OrientationComponent()
    {
        this->curDir = Dir::Down;
        this->lastDir = Dir::Down;
        this->visual = this->curDir.toVec();
    }
    
    Dir         curDir;
    Dir         lastDir;
    cc::Vec2    visual;
};