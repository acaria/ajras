#include "Headers.h"

void MeleeSystem::tick(double dt)
{
    for(auto eid : ecs.join<cp::Melee, cp::Collision, cp::Position, cp::Render>())
    {
        if (ecs::has<cp::Input>(eid) && ecs::get<cp::Input>(eid).disabled)
            continue;
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
        
        unsigned atkDir = Dir::kNone;
        
        if (cpMelee.launched)
            atkDir = cpMelee.curDir;
        else
        {
            atkDir = ecs::has<cp::Orientation>(eid) ? ecs::get<cp::Orientation>(eid).curDir : Dir::kNone;
        
            switch(cpMelee.type)
            {
                case MeleeComponent::SELF: {
                    atkDir = kNone;
                    break;
                }
                case MeleeComponent::DIR: {
                    if (ecs::has<cp::Orientation>(eid))
                        atkDir = ecs::get<cp::Orientation>(eid).curDir;
                    else
                        Log("invalid dir for meleecomponent type DIR");
                    break;
                }
                default:
                    Log("melee type not implemented");
                    break;
            }
        }
        
        cocos2d::Rect atkRect = getAtkRectFromDir(body, cpMelee.range, atkDir);

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
                auto cpRender2 = ecs::get<cp::Render>(oid);
                
                cocos2d::Rect bounds = {
                    cpPosition2.pos.x + cpCollision2.rect.origin.x,
                    cpPosition2.pos.y + cpCollision2.rect.origin.y,
                    cpCollision2.rect.size.width,
                    cpCollision2.rect.size.height
                };
                
                if (atkRect.intersectsRect(bounds))
                {
                    if (cpMelee.launched)
                    {
                        if (!cpMelee.processed)
                        {
                            ///detect striking
                            auto p = cpRender.elapsedTime / cpRender.getCurAnim()->duration();
                            if (p > 0.3 && p < 0.7)
                            {
                                //strike
                                cpMelee.processed = true;
                                
                                float duration = 1.f;
                                int   freq = 5;
                                
                                Log("strike=%u", oid);
                                
                                if (ecs::has<cp::Input>(oid))
                                    ecs::get<cp::Input>(oid).disable(duration);
                                cpRender2.sprite->runAction(Repeat::create(
                                    Sequence::create(TintTo::create(duration / freq / 2, 255, 50, 50),
                                                     TintTo::create(duration / freq / 2, 255, 255, 255),
                                                     NULL), freq));
                                
                                if (ecs::has<cp::Velocity>(oid))
                                {
                                    ecs::get<cp::Velocity>(oid).decelFactor = 1;
                                    ecs::get<cp::Velocity>(oid).velocity = (bounds.origin - body.origin).getNormalized() * 5;
                                    Log("x:%f,y:%f", ecs::get<cp::Velocity>(oid).velocity.x,
                                        ecs::get<cp::Velocity>(oid).velocity.y);
                                }
                            }
                        }
                    }
                    else
                    {
                        if (cpMelee.isCoolDown() && !cpRender.busy)
                        {
                            //launch
                            cpMelee.launch(atkDir);
                            auto animName = cpMelee.name + ProfileData::getTagName(atkDir);
                            cpRender.setAnimation(animName, 1, [&cpMelee](bool canceled){
                                cpMelee.launched = false;
                            });
                        }
                    }
                }
            }
        }
    }
}

cocos2d::Rect MeleeSystem::getAtkRectFromDir(const cocos2d::Rect& bounds, unsigned range, unsigned dir)
{
    switch(dir)
    {
        case Dir::kLeft:
            return cocos2d::Rect(bounds.getMinX() - range, bounds.getMidY() - range / 2, range, range);
        case Dir::kRight:
            return cocos2d::Rect(bounds.getMaxX(), bounds.getMidY() - range / 2, range, range);
        case Dir::kDown:
            return cocos2d::Rect(bounds.getMidX() - range / 2, bounds.getMinY() - range, range, range);
        case Dir::kUp:
            return cocos2d::Rect(bounds.getMidX() - range / 2, bounds.getMaxY(), range, range);
        case Dir::kNone:
            return cocos2d::Rect(bounds.getMinX() - range,
                                 bounds.getMinY() - range,
                                 bounds.size.width + (range << 1),
                                 bounds.size.height + (range << 1));
        default:
            return cocos2d::Rect::ZERO;
    }
}