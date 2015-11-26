#pragma once

#include "SweptAABB.h"
#include "Defines.h"

class CollisionEngine
{
    using ColCat = def::collision::Cat;
public:
    struct SweepInfo
    {
        float       count;
        cc::Point   dir;
        cc::Point   tx;
    };

    static lib::Box bounce(const cc::Rect& bounds,
                    const cc::Rect& lastBounds,
                    const cocos2d::Rect& target);
    
    static cc::Vec2 slide(const cc::Rect& src, const cc::Rect& target);
    static SweepInfo interpolateDir(const cc::Point& dir,
                                    const cc::Rect& bounds,
                                    const cc::Size& tileSize);
    static std::list<cc::Rect> mergeRectGrids(std::list<cc::Rect> src);
    
private:

    static bool needMerge(const cc::Rect& r1, const cc::Rect& r2);
};