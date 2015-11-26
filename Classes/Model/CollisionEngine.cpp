#include "CollisionEngine.h"

lib::Box CollisionEngine::bounce(const cc::Rect& bounds,
                                 const cc::Rect& lastBounds,
                                 const cocos2d::Rect& target)
{
    /*cocos2d::Vec2 pRes = cpPos.lastPos;
     auto b1 = lib::Box(cpPos.lastPos.x + cpCol.shape.getMinX(),
     cpPos.lastPos.y + cpCol.shape.getMinY(),
     cpCol.shape.size.width, cpCol.shape.size.height,
     cpPos.pos.x - cpPos.lastPos.x,
     cpPos.pos.y - cpPos.lastPos.y);
     auto b2 = lib::Box(target, {0, 0});
     
     float nX, nY;
     auto ct = lib::SweptAABB(b1, b2, nX, nY);
     auto translation = cc::Vec2(0,0);
     if (ct < 1.0f)
     {
     translation = {b1.vx * ct, b1.vy * ct};
     auto rt = 1.f - ct;
     b1.vx *= rt;
     b1.vy *= rt;
     if (fabs(nX) > 0.0001f) b1.vx = -b1.vx;
     if (fabs(nY) > 0.0001f) b1.vy = -b1.vy;
     translation += {b1.vx * rt, b1.vy * rt};
     }
     
     pRes += translation;
     return lib::Box(pRes.x, pRes.y, b1.w, b1.h, b1.vx, b1.vy);*/
    return lib::Box(0,0,0,0,0,0);

}

cc::Vec2 CollisionEngine::slide(const cc::Rect& src, const cc::Rect& target)
{
    cc::Rect intersect = {
        MAX(src.getMinX(), target.getMinX()),
        MAX(src.getMinY(), target.getMinY()),
        MIN(src.getMaxX(), target.getMaxX()) - MAX(src.getMinX(), target.getMinX()),
        MIN(src.getMaxY(), target.getMaxY()) - MAX(src.getMinY(), target.getMinY())
    };
    
    cocos2d::Vec2 result;
    if (intersect.size.width > intersect.size.height) // ySlide
    {
        if (intersect.getMinY() > src.getMinY())
            result.y = -intersect.size.height;
        else
            result.y = intersect.size.height;
    }
    else //xSlide
    {
        if (intersect.getMinX() > src.getMinX())
            result.x = -intersect.size.width;
        else
            result.x = intersect.size.width;
    }
    
    return result;
}

CollisionEngine::SweepInfo CollisionEngine::interpolateDir(const cc::Point& dir,
                                                           const cc::Rect& bounds,
                                                           const cc::Size& tileSize)
{
    SweepInfo sweep = {
        .count = 0,
        .dir = {0,0},
        .tx = dir
    };
    
    if (dir.x != 0 && (std::abs(dir.x) / bounds.size.width) > sweep.count)
    {
        sweep.count = std::abs(dir.x) / bounds.size.width;
        sweep.dir = {
            dir.x / sweep.count,
            dir.y * bounds.size.width / std::abs(dir.x)
        };
        sweep.tx = {
            fmodf(dir.x, sweep.dir.x),
            dir.y ? fmodf(dir.y, sweep.dir.y) : 0
        };
    }
    
    if (dir.y != 0 && (std::abs(dir.y) / bounds.size.height) > sweep.count)
    {
        sweep.count = std::abs(dir.y) / bounds.size.height;
        sweep.dir = {
            dir.x * bounds.size.height / std::abs(dir.y),
            dir.y / sweep.count,
        };
        sweep.tx = {
            dir.x ? fmodf(dir.x, sweep.dir.x) : 0,
            fmodf(dir.y, sweep.dir.y)
        };
    }
    
    auto tWidth = tileSize.width;
    if (dir.x != 0 && (std::abs(dir.x) / tWidth) > sweep.count)
    {
        sweep.count = std::abs(dir.x) / tWidth;
        sweep.dir = {
            dir.x / sweep.count,
            dir.y * tWidth / std::abs(dir.x) };
        sweep.tx = {
            fmodf(dir.x, sweep.dir.x),
            dir.y ? fmodf(dir.y, sweep.dir.y) : 0
        };
    }
    
    auto tHeight = tileSize.height;
    if (dir.y != 0 && (std::abs(dir.y) / tHeight) > sweep.count)
    {
        sweep.count = std::abs(dir.y) / tHeight;
        sweep.dir = {
            dir.x * tHeight / std::abs(dir.y),
            dir.y / sweep.count};
        sweep.tx = {
            dir.x ? fmodf(dir.x, sweep.dir.x) : 0,
            fmodf(dir.y, sweep.dir.y)
        };
    }
    
    return sweep;
}

bool CollisionEngine::needMerge(const cc::Rect &r1, const cc::Rect &r2)
{
    if (r1.size.width != r2.size.width && r1.size.height != r2.size.height)
        return false;
    if (r1.size.width == r2.size.width && r1.origin.x == r2.origin.x)
    {
        if (r1.getMinY() == r2.getMaxY() || r2.getMinY() == r1.getMaxY())
            return true;
    }
    if (r1.size.height == r2.size.height && r1.origin.y == r2.origin.y)
    {
        if (r1.getMinX() == r2.getMaxX() || r2.getMinX() == r1.getMaxX())
            return true;
    }
    
    return false;
}

std::list<cc::Rect> CollisionEngine::mergeRectGrids(std::list<cc::Rect> src)
{
    struct processInfo
    {
        cc::Rect bounds;
        bool processed;
    };
    
    std::list<processInfo> processList;
    for(auto bounds : src)
    {
        processList.push_back({
            .bounds = bounds,
            .processed = false
        });
    }
    
    std::list<cc::Rect> res;
    
    for(auto& info : processList)
    {
        if (info.processed)
            continue;
        cc::Rect mRect = info.bounds;
        for(auto& info2 : processList)
        {
            auto bounds = info2.bounds;
            if (needMerge(bounds, mRect))
            {
                info2.processed = true;
                cc::Point origin = {
                    MIN(bounds.origin.x, mRect.origin.x),
                    MIN(bounds.origin.y, mRect.origin.y)
                };
                mRect = {
                    origin.x, origin.y,
                    MAX(bounds.getMaxX(), mRect.getMaxX()) - origin.x,
                    MAX(bounds.getMaxY(), mRect.getMaxY()) - origin.y
                };
            }
        }
        res.push_back(mRect);
        info.processed = true;
    }
    
    return res;
}
