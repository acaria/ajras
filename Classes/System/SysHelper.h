#pragma once
#include "PositionComponent.h"
#include "PhysicsComponent.h"
#include "AIComponent.h"

class SysHelper
{
public:
    static cocos2d::Rect getBounds(const PositionComponent& position,
                                   const PhysicsComponent& collision);
    
    static cocos2d::Rect getLastBounds(const PositionComponent& position,
                                       const PhysicsComponent& collision);
    
    static cocos2d::Rect getBounds(unsigned eid);
    
    static unsigned getNearest(unsigned gid,unsigned eid,
                               def::mood::Flags moodCat,
                               float maxLength);
};