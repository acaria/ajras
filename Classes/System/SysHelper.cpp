#include "SysHelper.h"
#include "Components.h"

cc::Rect SysHelper::getBounds(const PositionComponent &position,
                              const CollisionComponent &collision)
{
    return cc::Rect(
        position.pos.x + collision.rect.origin.x,
        position.pos.y + collision.rect.origin.y,
        collision.rect.size.width - 1,
        collision.rect.size.height - 1
    );
}

cc::Rect SysHelper::getLastBounds(const PositionComponent &position,
                                  const CollisionComponent &collision)
{
    return cc::Rect(
        position.last.x + collision.rect.origin.x,
        position.last.y + collision.rect.origin.y,
        collision.rect.size.width - 1,
        collision.rect.size.height - 1);
}

cc::Rect SysHelper::getBounds(unsigned eid)
{
    CCASSERT((ecs::has<cp::Position, cp::Collision>(eid)), "invalid entity for bounds processing");
    return SysHelper::getBounds(ecs::get<cp::Position>(eid), ecs::get<cp::Collision>(eid));
}

unsigned SysHelper::getNearest(unsigned gid, unsigned int eid, AIComponent::eMood mood, float maxLength)
{
    auto maxDist = maxLength * maxLength;
    auto bounds = SysHelper::getBounds(eid);
    
    float nearest = maxDist;
    unsigned targetID = 0;
    for(auto tid : ecs::join<cp::AI, cp::Position, cp::Collision>(gid))
    {
        if (tid == eid)
            continue;
        if (mood != ecs::get<cp::AI>(tid).mood)
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