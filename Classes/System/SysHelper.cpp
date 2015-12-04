#include "Defines.h"
#include "SysHelper.h"

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

lib::Nullable<unsigned> SysHelper::getNearest(unsigned gid, unsigned int eid, def::mood::Flags moodCat, float maxLength)
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
    
    if (targetID == 0)
        return nullptr;

    return targetID;
}

def::collision::Agent SysHelper::makeAgent(unsigned eid)
{
    CCASSERT((ecs::has<cp::Position, cp::Physics>(eid)), "invalid entity");
    auto& cpPos = ecs::get<cp::Position>(eid);
    auto& cpPhy = ecs::get<cp::Physics>(eid);
    return def::collision::Agent {
        .id = eid,
        .bounds = getBounds(cpPos, cpPhy),
        .lastBounds = getLastBounds(cpPos, cpPhy),
        .category = cpPhy.category,
        .velocity = cpPhy.velocity
    };
}

void SysHelper::enableEntity(unsigned group, unsigned eid)
{
    CCASSERT((ecs::has<cp::Render, cp::Physics>(eid)), "invalid entity");
    auto& cpRender = ecs::get<cp::Render>(eid);
    cpRender.busy = false;
    ecs::add<cp::Position>(eid, group).set(cpRender.sprite->getPosition());
    
    if (ecs::has<cp::Input>(eid))
        ecs::get<cp::Input>(eid).enabled = true;
}

void SysHelper::disableEntity(unsigned group, unsigned int eid)
{
    CCASSERT((ecs::has<cp::Position, cp::Render, cp::Physics>(eid)), "invalid entity");
    auto& cpRender = ecs::get<cp::Render>(eid);
    auto& cpPhy = ecs::get<cp::Physics>(eid);
    cpPhy.resetForces();
    cpRender.busy = true;
    ecs::del<cp::Position>(eid, group);
    if (ecs::has<cp::Input>(eid))
        ecs::get<cp::Input>(eid).enabled = false;
}