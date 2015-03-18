#pragma once
#include "Headers.h"

class SysHelper
{
public:
    static cocos2d::Rect getBounds(const PositionComponent& position,
                                   const CollisionComponent& collision);
    
    static cocos2d::Rect getBounds(unsigned eid);
};