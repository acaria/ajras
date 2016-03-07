#pragma once
#include "Components.h"
#include "PlayerEntity.h"
#include "Defines.h"

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
    
    static lib::Nullable<unsigned> getNearest(unsigned gid,unsigned eid,
                                              def::mood::Flags moodCat,
                                              float maxLength);
    
    static unsigned findTeamLeaderId(unsigned gid, unsigned eid, unsigned teamIndex);
    static std::set<unsigned> findTeamIds(unsigned gid, unsigned eid, unsigned teamIndex);
    
    static def::collision::Agent makeAgent(unsigned eid);
    
    static void enableEntity(unsigned gid, unsigned eid);
    static void disableEntity(unsigned gid, unsigned eid);
    
    static unsigned createPlayerEntity(LayeredContainer* parent,
                                       unsigned group,
                                       cc::Vec2 srcPos,
                                       PlayerEntity entity);
};