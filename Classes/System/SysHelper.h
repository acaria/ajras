#pragma once
#include "Components.h"
#include "PlayerEntity.h"
#include "Defines.h"
#include "Random.h"
#include "SystemDispatcher.h"

class SysHelper
{
public:
    static cocos2d::Rect getBounds(const PositionComponent& position,
                                   const PhysicsComponent& collision);
    
    static cocos2d::Rect getLastBounds(const PositionComponent& position,
                                       const PhysicsComponent& collision);
    
    static cocos2d::Rect getBounds(unsigned eid);
    
    static float getDistSquared(unsigned eid1, unsigned eid2);
    static float getDist(unsigned eid1, unsigned eid2);
    
    static SlotData* getAvailableSlot(unsigned agentID, CollectibleData* collectible);
    static bool getCurrencyAvailable(unsigned agentID, CollectibleData* collectible);
    
    static lib::Nullable<unsigned> getNearest(unsigned gid,unsigned eid,
                                              def::mood::Flags moodCat,
                                              float maxLength);
    
    static unsigned findTeamLeaderId(unsigned gid, unsigned eid, unsigned teamIndex);
    static std::set<unsigned> findTeamIds(unsigned gid, unsigned teamIndex);
    
    static def::collision::Agent makeAgent(unsigned eid);
    
    static void enableEntity(unsigned gid, unsigned eid);
    static void disableEntity(unsigned gid, unsigned eid);
    
    static unsigned createPlayerEntity(LayeredContainer* parent,
                                       unsigned group,
                                       const cc::Vec2& srcPos,
                                       const PlayerEntity& entity);
    
    static unsigned createEntity(LayeredContainer* parent,
                                 unsigned group,
                                 const cc::Vec2& srcPos,
                                 const std::string& profile,
                                 lib::Random& rEngine,
                                 SystemDispatcher& dispatcher);
    
    static void triggerHostileMode(unsigned gid);
};