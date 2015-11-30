#pragma once
#include "Dir.h"

struct OrientationComponent
{
    OrientationComponent() {
        dir = Dir::None;
        lastDir = Dir::None;
    }
    
    void set(const Dir& dir)
    {
        this->dir = dir;
        this->lastDir = dir;
    }
    
    Dir dir;
    Dir lastDir;
};