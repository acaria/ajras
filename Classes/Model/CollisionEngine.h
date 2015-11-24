#pragma once

#include "SweptAABB.h"

class CollisionEngine
{
public:
    static lib::Box bounce(const cc::Rect& bounds,
                    const cc::Rect& lastBounds,
                    const cocos2d::Rect& target);
    
    static cc::Vec2 slide(const cc::Rect& src, const cc::Rect& target);
};