#pragma once
#include "PositionComponent.h"
#include "CollisionComponent.h"

class SysHelper
{
public:
    static cocos2d::Rect getBounds(const PositionComponent& position,
                                   const CollisionComponent& collision);
    
    static cocos2d::Rect getBounds(unsigned eid);
};