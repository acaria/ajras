#include "Headers.h"

cocos2d::Rect SysHelper::getBounds(const PositionComponent &position,
                                   const CollisionComponent &collision)
{
    return Rect(
        position.pos.x + collision.rect.origin.x,
        position.pos.y + collision.rect.origin.y,
        collision.rect.size.width,
        collision.rect.size.height
    );
}

cocos2d::Rect SysHelper::getBounds(unsigned eid)
{
    CCASSERT((ecs::has<cp::Position, cp::Collision>(eid)), "invalid entity for bounds processing");
    return SysHelper::getBounds(ecs::get<cp::Position>(eid), ecs::get<cp::Collision>(eid));
}