#include "Defines.h"
#include "SysHelper.h"
#include "ModelProvider.h"

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
    CCASSERT((ecs::has<cp::Render, cp::Input, cp::Physics>(eid)), "invalid entity");
    auto& cpRender = ecs::get<cp::Render>(eid);
    cpRender.busy = false;
    ecs::get<cp::Position>(eid).set(cpRender.sprite->getPosition());
    
    ecs::get<cp::Input>(eid).enabled = true;
    ecs::get<cp::Input>(eid).withCollision = true;
}

void SysHelper::disableEntity(unsigned group, unsigned int eid)
{
    CCASSERT((ecs::has<cp::Position, cp::Render, cp::Physics>(eid)), "invalid entity");
    auto& cpRender = ecs::get<cp::Render>(eid);
    auto& cpPhy = ecs::get<cp::Physics>(eid);
    cpPhy.resetForces();
    cpRender.busy = true;
    
    ecs::get<cp::Input>(eid).enabled = false;
    ecs::get<cp::Input>(eid).withCollision = false;
}

unsigned SysHelper::createPlayerEntity(LayeredContainer* parent,
                                       unsigned group,
                                       cc::Vec2 srcPos,
                                       PlayerEntity entity)
{
    auto eid = cp::entity::genID();
    auto profile = ModelProvider::instance()->profile.get(entity.profileName);
    
    
    auto& cpRender = ecs::add<cp::Render>(eid, group);
    auto& cpPhy = ecs::add<cp::Physics>(eid, group);
    
    cpRender.setProfile(profile, parent);
    cpPhy.setProfile(profile);
    
    cc::Vec2 pos = {
        srcPos.x - cpPhy.shape.getMidX(),
        srcPos.y - cpPhy.shape.getMidY()
    };
    
    ecs::add<cp::Orientation>(eid, group);
    ecs::add<cp::Mood>(eid, group) = profile->getMood();
    ecs::add<cp::Melee>(eid, group).setProfile(profile);
    if (entity.ctrlIndex != 0)
        ecs::add<cp::Control>(eid, group) = entity.ctrlIndex;
    else
        ecs::add<cp::AI>(eid, group).setProfile(profile);
        //ecs::add<cp::Control>(eid, group) = 2;
    ecs::add<cp::Gear>(eid, group) = entity.inventory;
    ecs::add<cp::Stamina>(eid, group).setProfile(profile);
    ecs::add<cp::Health>(eid, group).setProfile(profile);
    ecs::add<cp::Input>(eid, group);
    ecs::add<cp::Position>(eid, group).set(pos);
    ecs::add<cp::Team>(eid, group) = def::PTEAM;
    
    cpRender.sprite->setPosition(pos);
    cpRender.sprite->setOpacity(0);
    
    return eid;
}
