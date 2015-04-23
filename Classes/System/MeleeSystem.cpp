#include "MeleeSystem.h"
#include "Components.h"
#include "AnimationData.h"
#include "GameCtrl.h"
#include "GameScene.h"
#include "ControlSystem.h"

void MeleeSystem::tick(double dt)
{
    for(auto eid : ecs.join<cp::Melee, cp::Collision, cp::Position, cp::Render, cp::Input>())
    {
        auto& cpInput = ecs::get<cp::Input>(eid);
        if (cpInput.disabled)
            continue;
        if (cpInput.actionMode != ActionMode::melee)
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
        
        Dir atkDir = Dir::None;
        
        if (cpMelee.launched)
            atkDir = cpMelee.curDir;
        else
        {
            atkDir = ecs::has<cp::Orientation>(eid) ? ecs::get<cp::Orientation>(eid).curDir : Dir::None;
        
            switch(cpMelee.type)
            {
                case MeleeComponent::SELF: {
                    atkDir = Dir::None;
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

        if (atkRect.equals(cc::Rect::ZERO))
            continue;
        
#if kDrawDebug
        cpRender.melee->setVisible(true);
        cpRender.melee->setPosition(atkRect.origin.x - cpPosition.pos.x, atkRect.origin.y - cpPosition.pos.y);
        cpRender.melee->setScale(atkRect.size.width, atkRect.size.height);
#endif
        
        //check room objects
        for(auto oid : ecs.join<cp::Render, cp::Collision, cp::Position, cp::Health>())
        {
            if (oid == eid) continue;
            
            if (!ecs::has<cp::Target>(eid) || ecs::get<cp::Target>(eid) == oid)
            {
                auto &cpPosition2 = ecs::get<cp::Position>(oid);
                auto &cpCollision2 = ecs::get<cp::Collision>(oid);
                auto &cpRender2 = ecs::get<cp::Render>(oid);
                auto &cpHealth2 = ecs::get<cp::Health>(oid);
                
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
                            auto anim = cpRender.getCurAnim();
                            auto p = cpRender.elapsedTime / anim->duration();
                            if (p > 0.3 && p < 0.7)
                            {
                                //strike
                                cpMelee.processed = true;
                                
                                float duration = 1.f;
                                int   freq = 5;
                                
                                if (ecs::has<cp::Input>(oid))
                                    ecs::get<cp::Input>(oid).disable(duration);
                                cpRender2.runAction(cc::Repeat::create(
                                    cc::Sequence::create(cc::TintTo::create(duration / freq / 2, 255, 50, 50),
                                    cc::TintTo::create(duration / freq / 2, 255, 255, 255),
                                    NULL), freq));
                                
                                if (ecs::has<cp::Velocity>(oid))
                                {
                                    ecs::get<cp::Velocity>(oid).decelFactor = 1;
                                    ecs::get<cp::Velocity>(oid).velocity = (bounds.origin - body.origin).getNormalized() * 5;
                                }
                                
                                cpHealth2.hp -= cpMelee.damage;
                                this->onHealthChanged(oid, cpHealth2.hp);

#if kDrawInfo
                                cpRender2.lInfo->setString(std::to_string(cpHealth2.hp));
#endif
                                if (cpHealth2.hp == 0)
                                {
                                    ecs.del<cp::Target>(eid);
                                    //ecs.del<cp::Input>(oid);
                                    cpRender2.setAnimation("death", 1, [oid, this](bool cancel){
                                        cp::entity::remove(oid, ecs.getID());
                                    });
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

cocos2d::Rect MeleeSystem::getAtkRectFromDir(const cocos2d::Rect& bounds, unsigned range, const Dir& dir)
{
    switch(dir.getRaw())
    {
        case Dir::Left:
            return cocos2d::Rect(bounds.getMinX() - range, bounds.getMidY() - range / 2, range, range);
        case Dir::Right:
            return cocos2d::Rect(bounds.getMaxX(), bounds.getMidY() - range / 2, range, range);
        case Dir::Down:
            return cocos2d::Rect(bounds.getMidX() - range / 2, bounds.getMinY() - range, range, range);
        case Dir::Up:
            return cocos2d::Rect(bounds.getMidX() - range / 2, bounds.getMaxY(), range, range);
        case Dir::None:
            return cocos2d::Rect(bounds.getMinX() - range,
                                 bounds.getMinY() - range,
                                 bounds.size.width + (range << 1),
                                 bounds.size.height + (range << 1));
        default:
            return cocos2d::Rect::ZERO;
    }
}