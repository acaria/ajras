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
