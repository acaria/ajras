#pragma once
#include "Components.h"
#include "Defines.h"

class SysHelper
{
public:
    static cocos2d::Rect getBounds(const PositionComponent& position,
                                   const PhysicsComponent& collision);
    
    static cocos2d::Rect getLastBounds(const PositionComponent& position,
                                       const PhysicsComponent& collision);
    
    static cocos2d::Rect getBounds(unsigned eid);
    
    static lib::Nullable<unsigned> getNearest(unsigned gid,unsigned eid,
                                              def::mood::Flags moodCat,
                                              float maxLength);
    
    static def::collision::Agent makeAgent(unsigned eid);
    
    static void enableEntity(unsigned group, unsigned eid);
    static void disableEntity(unsigned group, unsigned eid);
};