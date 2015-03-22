#pragma once
#include "Headers.h"

struct PositionComponent
{
    PositionComponent() {
        pos = {0, 0};
        last = {0, 0};
    }
    
    void set(const cocos2d::Vec2 &pos)
    {
        this->pos = pos;
        this->last = pos;
    }
    
    cocos2d::Vec2 pos;
    cocos2d::Vec2 last;
};