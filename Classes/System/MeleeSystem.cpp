#include "Headers.h"

void MeleeSystem::tick(double dt)
{
    for(auto eid : ecs.join<cp::Melee, cp::Collision, cp::Position, cp::Render>())
    {
        auto& cpPosition = ecs::get<cp::Position>(eid);
        auto& cpCollision = ecs::get<cp::Collision>(eid);
        auto& cpRender = ecs::get<cp::Render>(eid);
        auto& cpMelee = ecs::get<cp::Melee>(eid);
        
        cocos2d::Rect body = {
            cpPosition.pos.x + cpCollision.rect.origin.x,
            cpPosition.pos.y + cpCollision.rect.origin.y,
            cpCollision.rect.size.width,
            cpCollision.rect.size.height
        };
        
        cocos2d::Rect atkRect = cocos2d::Rect::ZERO;
        unsigned atkDir = ecs::has<cp::Orientation>(eid) ? ecs::get<cp::Orientation>(eid).curDir : Dir::kNone;
        
        switch(cpMelee.type)
        {
            case MeleeComponent::SELF: {
                atkRect = cocos2d::Rect(body.getMinX() - cpMelee.range,
                                        body.getMinY() - cpMelee.range,
                                        body.size.width + (cpMelee.range << 1),
                                        body.size.height + (cpMelee.range << 1));
                break;
            }
            case MeleeComponent::DIR: {
                if (ecs::has<cp::Orientation>(eid))
                {
                    atkDir = ecs::get<cp::Orientation>(eid).curDir;
                    switch(atkDir)
                    {
                        case Dir::kLeft:
                            atkRect = cocos2d::Rect(body.getMinX() - cpMelee.range, body.getMidY() - cpMelee.range / 2, cpMelee.range, cpMelee.range);
                            break;
                        case Dir::kRight:
                            atkRect = cocos2d::Rect(body.getMaxX(), body.getMidY() - cpMelee.range / 2, cpMelee.range, cpMelee.range);
                            break;
                        case Dir::kDown:
                            atkRect = cocos2d::Rect(body.getMidX() - cpMelee.range / 2, body.getMinY() - cpMelee.range, cpMelee.range, cpMelee.range);
                            break;
                        case Dir::kUp:
                            atkRect = cocos2d::Rect(body.getMidX() - cpMelee.range / 2, body.getMaxY(), cpMelee.range, cpMelee.range);
                            break;
                        default:
                            break; //do nothing
                    }
                }
                break;
            }
            default:
                Log("melee type not implemented");
                break;
        }
        
        if (atkRect.equals(Rect::ZERO))
            continue;
        
#if kDrawDebug
        cpRender.melee->setVisible(true);
        cpRender.melee->setPosition(atkRect.origin.x - cpPosition.pos.x, atkRect.origin.y - cpPosition.pos.y);
        cpRender.melee->setScale(atkRect.size.width, atkRect.size.height);
#endif
        
        //check room objects
        for(auto oid : ecs.join<cp::Render, cp::Collision, cp::Position>())
        {
            if (oid != eid)
            {
                auto cpPosition2 = ecs::get<cp::Position>(oid);
                auto cpCollision2 = ecs::get<cp::Collision>(oid);
                
                cocos2d::Rect bounds = {
                    cpPosition2.pos.x + cpCollision2.rect.origin.x,
                    cpPosition2.pos.y + cpCollision2.rect.origin.y,
                    cpCollision2.rect.size.width,
                    cpCollision2.rect.size.height
                };
                
                if (atkRect.intersectsRect(bounds) &&
                    cpMelee.isCoolDown() &&
                    !cpRender.busy)
                {
                    cpMelee.lastTime = lib::now();
                    auto animName = cpMelee.name + ProfileData::getTagName(atkDir);
                    cpRender.setAnimation(animName, 1);
                }
            }
        }
    }
}