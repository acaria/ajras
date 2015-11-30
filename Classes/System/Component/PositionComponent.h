#pragma once
#include "Dir.h"

struct PositionComponent
{
    PositionComponent() {
        pos = {0, 0};
        lastPos = {0, 0};
    }
    
    void set(const cc::Vec2 &pos)
    {
        this->pos = pos;
        this->lastPos = pos;
    }
    
    cc::Vec2    pos;
    cc::Vec2    lastPos;
};