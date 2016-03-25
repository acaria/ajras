#include "Defines.h"
#include "SysHelper.h"
#include "ModelProvider.h"
#include "GameCtrl.h"

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

float SysHelper::getDistSquared(unsigned eid1, unsigned eid2)
{
    auto bounds1 = SysHelper::getBounds(eid1);
    auto bounds2 = SysHelper::getBounds(eid2);
    
    return cc::Point(bounds2.getMidX() - bounds1.getMidX(),
                     bounds2.getMidY() - bounds1.getMidY()).lengthSquared();
}

float SysHelper::getDist(unsigned eid1, unsigned eid2)
{
    auto bounds1 = SysHelper::getBounds(eid1);
    auto bounds2 = SysHelper::getBounds(eid2);
    
    return cc::Point(bounds2.getMidX() - bounds1.getMidX(),
                     bounds2.getMidY() - bounds1.getMidY()).length();
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

unsigned SysHelper::findTeamLeaderId(unsigned gid, unsigned eid, unsigned teamIndex)
{
    for(auto oid : ecs::join<cp::Team, cp::Physics, cp::Position>(gid))
    {
        auto& cpTeam = ecs::get<cp::Team>(oid);
        if (cpTeam.index == teamIndex && cpTeam.position == 0)
        {
            return oid;
        }
    }
    return 0;
}

std::set<unsigned> SysHelper::findTeamIds(unsigned gid, unsigned teamIndex)
{
    std::set<unsigned> result;
    for(auto oid : ecs::join<cp::Team, cp::Physics, cp::Position>(gid))
    {
        auto& cpTeam = ecs::get<cp::Team>(oid);
        if (cpTeam.index == teamIndex)
            result.insert(oid);
    }
    return result;
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

void SysHelper::enableEntity(unsigned gid, unsigned eid)
{
    CCASSERT((ecs::has<cp::Render, cp::Input, cp::Physics>(eid)), "invalid entity");
    auto& cpRender = ecs::get<cp::Render>(eid);
    cpRender.busy = false;
    ecs::add<cp::Position>(eid, gid).set(cpRender.sprite->getPosition());
    ecs::get<cp::Input>(eid).enabled = true;
}

void SysHelper::disableEntity(unsigned gid, unsigned int eid)
{
    CCASSERT((ecs::has<cp::Render, cp::Input, cp::Physics>(eid)), "invalid entity");
    ecs::del<cp::Position>(eid, gid);
    auto& cpRender = ecs::get<cp::Render>(eid);
    auto& cpPhy = ecs::get<cp::Physics>(eid);
    cpPhy.resetForces();
    cpRender.busy = true;
    
    ecs::get<cp::Input>(eid).enabled = false;
}

bool SysHelper::getCurrencyAvailable(unsigned agentID, CollectibleData* collectible)
{
    if (!ecs::has<cp::Gear>(agentID) || collectible == nullptr || !collectible->currency)
        return false;
    
    auto& cpGear = ecs::get<cp::Gear>(agentID);
    
    if (cpGear.currency.stackability == -1 ||
        (cpGear.currency.stackability - cpGear.currency.quantity) >= collectible->currencyValue)
        return true;
    return false;
}

SlotData* SysHelper::getAvailableSlot(unsigned agentID, CollectibleData* collectible)
{
    if (!ecs::has<cp::Gear>(agentID) || collectible == nullptr)
        return nullptr;
    
    auto& cpGear = ecs::get<cp::Gear>(agentID);
    
    if (collectible->currency)
        return nullptr; // no currency stored in slotData, use slotcurrency instead
    
    for(auto& slot : cpGear.slots)
    {
        if (slot.quantity == 0)
            continue;
        if (slot.content->key == collectible->key && slot.quantity < slot.content->stackability)
        {
            slot.quantity++;
            return &slot;
        }
    }
    
    for(auto& slot : cpGear.slots)
    {
        if (slot.quantity == 0)
        {
            slot.quantity = 1;
            slot.content = collectible;
            return &slot;
        }
    }
    
    return nullptr;
}

unsigned SysHelper::createPlayerEntity(LayeredContainer* parent,
                                       unsigned group,
                                       const cc::Vec2& srcPos,
                                       const PlayerEntity& entity)
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
    {
        //control
        ecs::add<cp::Control>(eid, group) = entity.ctrlIndex;
    }
    else
    {
        ecs::add<cp::AI>(eid, group).setProfile(profile);
        //ecs::add<cp::Control>(eid, group) = 2;
    }
    ecs::add<cp::Gear>(eid, group).slots = entity.inventory;
    ecs::add<cp::Gear>(eid, group).currency = entity.currency;
    ecs::add<cp::Stamina>(eid, group).setProfile(profile);
    ecs::add<cp::Health>(eid, group).setProfile(profile);
    ecs::add<cp::Input>(eid, group);
    ecs::add<cp::Position>(eid, group).set(pos);
    ecs::add<cp::Team>(eid, group) = entity.team;
    ecs::add<cp::Trail>(eid, group).set(3);
    
    cpRender.sprite->setPosition(pos);
    cpRender.sprite->setOpacity(0);
    
    return eid;
}

unsigned SysHelper::createEntity(LayeredContainer* parent,
                                 unsigned group,
                                 const cc::Vec2& srcPos,
                                 const std::string& profileName,
                                 lib::Random& random, SystemDispatcher& dispatcher)
{
    auto eid = cp::entity::genID();
    auto profile = ModelProvider::instance()->profile.get(profileName);
    
    
    auto& cpRender = ecs::add<cp::Render>(eid, group);
    auto& cpPhy = ecs::add<cp::Physics>(eid, group);
    
    cc::Vec2 pos = {
        srcPos.x - cpPhy.shape.getMidX(),
        srcPos.y - cpPhy.shape.getMidY()
    };
    
    cpRender.setProfile(profile, parent);
    cpRender.sprite->setPosition(pos);
    cpRender.sprite->setOpacity(0);
    cpRender.sprite->setCascadeOpacityEnabled(true);
    
    cpPhy.setProfile(profile);
    
    ecs::add<cp::Mood>(eid, group) = profile->getMood();
    ecs::add<cp::Input>(eid, group);
    ecs::add<cp::Position>(eid, group).set(pos);
    
    if (profile->stats != nullptr)
    {
        auto stats = profile->stats.Value;
        
        if (stats.orientation)
        {
            ecs::add<cp::Orientation>(eid, group);
        }
        
        if (stats.melee != nullptr)
        {
            ecs::add<cp::Melee>(eid, group).setProfile(profile);
            ecs::add<cp::Stamina>(eid, group);
        }
        
        if (stats.stamina != nullptr)
        {
            ecs::add<cp::Stamina>(eid, group).setProfile(profile);
        }
        
        if (stats.health != nullptr)
        {
            ecs::add<cp::Health>(eid, group).setProfile(profile);
        }
    }
    
    if (profile->behaviour != nullptr)
    {
        ecs::add<cp::AI>(eid, group).setProfile(profile);
    }
    
    if (profile->interaction != nullptr)
    {
        ecs::add<cp::Interact>(eid, group).setProfile(profile);
        
        switch(lib::hash(profile->interaction->actionType))
        {
            case lib::hash("reward"): {
                assert(profile->interaction->actionParams != nullptr);
                auto collectables = ModelProvider::instance()->collectible.genReward(
                        random, profile->interaction->actionParams.Value);
                GearComponent reward;
                for(auto collectable : collectables)
                {
                    reward.slots.push_back(SlotData {
                        .quantity = 1,
                        .content = collectable
                    });
                }
                ecs::add<cp::Gear>(eid, group) = reward;
            }
            break;
            case lib::hash("light"): {
                assert(profile->interaction->actionParams != nullptr);
                auto lightType = profile->interaction->actionParams.Value;
                auto& light = GameCtrl::instance()->getLight();
                auto& cpLight = ecs::add<cp::Light>(eid, group);
                cpLight.defaultColor = light.defaultConfig.objects[lightType].color;
                cpLight.defaultSize = light.defaultConfig.objects[lightType].size;

                cpLight.halo = cc::Sprite::createWithSpriteFrameName("grad_circle.png");
                cpLight.halo->setPosition({cpPhy.shape.getMidX() + pos.x, cpPhy.shape.getMidY() + pos.y});
                cpLight.halo->setOpacity(160);
                cpLight.halo->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
                cpLight.halo->setScale(0,0);
                cpLight.halo->setColor(cpLight.defaultColor.first);
                parent->add(cpLight.halo, def::LayerType::FG, {0, 0});
            }
            break;
            default:
            {
                Log("unknown interaction action type: %s",
                    profile->interaction->actionType.c_str());
            }
            break;
        }
    }
    
    dispatcher.onEntityAdded(group, eid);
    return eid;
}
