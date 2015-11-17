#include "SysHelper.h"
#include "Components.h"

cc::Rect SysHelper::getBounds(const PositionComponent &position,
                              const PhysicsComponent &collision)
{
    return cc::Rect(
        position.pos.x + collision.shape.origin.x,
        position.pos.y + collision.shape.origin.y,
        collision.shape.size.width,
        collision.shape.size.height
    );
}

cc::Rect SysHelper::getLastBounds(const PositionComponent &position,
                                  const PhysicsComponent &collision)
{
    return cc::Rect(
        position.lastPos.x + collision.shape.origin.x,
        position.lastPos.y + collision.shape.origin.y,
        collision.shape.size.width,
        collision.shape.size.height);
}

cc::Rect SysHelper::getBounds(unsigned eid)
{
    CCASSERT((ecs::has<cp::Position, cp::Physics>(eid)), "invalid entity for bounds processing");
    return SysHelper::getBounds(ecs::get<cp::Position>(eid), ecs::get<cp::Physics>(eid));
}

unsigned SysHelper::getNearest(unsigned gid, unsigned int eid, def::mood::Flags moodCat, float maxLength)
{
    auto maxDist = maxLength * maxLength;
    auto bounds = SysHelper::getBounds(eid);
    
    float nearest = maxDist;
    unsigned targetID = 0;
    for(auto tid : ecs::join<cp::AI, cp::Position, cp::Physics, cp::Mood>(gid))
    {
        if (tid == eid)
            continue;
        if (!def::mood::inside(moodCat, ecs::get<cp::Mood>(tid)))
            continue;
        auto bounds2 = SysHelper::getBounds(tid);
        float dist = cc::Vec2(bounds.getMidX() - bounds2.getMidX(),
                              bounds.getMidY() - bounds2.getMidY()).lengthSquared();
        if (dist < maxDist && dist < nearest)
        {
            nearest = dist;
            targetID = tid;
        }
    }

    return targetID;
}