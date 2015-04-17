#include "SysHelper.h"
#include "Components.h"

cc::Rect SysHelper::getBounds(const PositionComponent &position,
                                   const CollisionComponent &collision)
{
    return cc::Rect(
        position.pos.x + collision.rect.origin.x,
        position.pos.y + collision.rect.origin.y,
        collision.rect.size.width - 1,
        collision.rect.size.height - 1
    );
}

cc::Rect SysHelper::getBounds(unsigned eid)
{
    CCASSERT((ecs::has<cp::Position, cp::Collision>(eid)), "invalid entity for bounds processing");
    return SysHelper::getBounds(ecs::get<cp::Position>(eid), ecs::get<cp::Collision>(eid));
}