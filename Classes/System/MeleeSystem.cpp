#include "MeleeSystem.h"
#include "Components.h"
#include "AnimationData.h"
#include "MissionScene.h"
#include "ControlSystem.h"
#include "SysHelper.h"
#include "GameCtrl.h"
#include "IMapData.h"
#include "CocosHelper.h"

void MeleeSystem::tick(double dt)
{
    std::list<std::list<std::pair<unsigned, unsigned>>> meleeListGroup;
    for(auto eid : context->ecs->join<cp::Melee, cp::Mood, cp::Stamina, cp::Physics, cp::Position, cp::Render>())
    {
        if (!ecs::has<cp::Input>(eid) || !ecs::get<cp::Input>(eid).isActive())
            continue;
        
        auto entityMood = ecs::get<cp::Mood>(eid);
        auto oppositeMoods = def::mood::getOpponents(entityMood);
        
        auto& cpMelee = ecs::get<cp::Melee>(eid);
        if (!cpMelee.enabled || cpMelee.type == MeleeComponent::NONE)
            continue;
        
        cocos2d::Rect body = SysHelper::getBounds(eid);
        
        Dir atkDir = getAtkDir(eid, cpMelee);
        cpMelee.atkRect = getAtkRectFromDir(body, cpMelee.range, atkDir);

        if (cpMelee.atkRect.size.width == 0 || cpMelee.atkRect.size.height == 0)
            continue;
        
        //check room objects
        for(auto oid : context->ecs->join<cp::Render, cp::Physics, cp::Position, cp::Health, cp::Mood, cp::Input>())
        {
            if (oid == eid) continue;
            
            if (ecs::has<cp::Untargetable>(oid))
                continue;
            //if (ecs::has<cp::Melee>(oid) && !ecs::get<cp::Melee>(oid).enabled)
            //    continue;
            
            //check disabled input for oid????
            
            if ((!ecs::has<cp::Target>(eid) || ecs::get<cp::Target>(eid) == oid) &&
                def::mood::inside(ecs::get<cp::Mood>(oid), oppositeMoods))
            {
                cc::Rect bounds = SysHelper::getBounds(oid);
                
                if (cpMelee.atkRect.intersectsRect(bounds) &&
                    ecs::get<cp::Stamina>(eid).current >= cpMelee.stamina)
                {
                    if (context->data->getCol()->checkCollisionRay({body.getMidX(), body.getMidY()},
                                                           {bounds.getMidX(), bounds.getMidY()},
                                                           ecs::get<cp::Physics>(eid).category))
                        continue;
                        
                    
                    bool added = false;
                    for(auto& meleeList : meleeListGroup)
                    {
                        for(auto pair : meleeList)
                        {
                            if (pair.first == oid || pair.first == eid || pair.second == oid || pair.second == eid)
                            {
                                added = true;
                                meleeList.push_back({eid, oid});
                                break;
                            }
                        }
                    }
                    if (!added)
                        meleeListGroup.push_back({{eid, oid}});
                }
            }
        }
    }
    
    //conflicts resolution
    for(auto& meleeList : meleeListGroup)
    {
        if (meleeList.size() > 1)
        {
            float best = 0;
            std::pair<unsigned, unsigned> selection;
            for(auto pair : meleeList)
            {
                auto& cpStamina = ecs::get<cp::Stamina>(pair.first);
                auto& cpMelee = ecs::get<cp::Melee>(pair.first);
                if (cpStamina.current - cpMelee.stamina >= best)
                {
                    best = cpStamina.current - cpMelee.stamina;
                    selection = pair;
                }
            }
            meleeList = {selection};
        }
    }
    
    for(auto meleeList : meleeListGroup)
    {
        assert(meleeList.size() <=  1);
        auto pair = meleeList.front();
        auto& cpMelee = ecs::get<cp::Melee>(pair.first);
        ecs::get<cp::Stamina>(pair.first).current -= cpMelee.stamina;
        switch(cpMelee.type)
        {
            case MeleeComponent::DIR:
                this->processDirMelee(pair.first, pair.second, getAtkDir(pair.first, cpMelee));
                break;
            case MeleeComponent::CHARGE:
            case MeleeComponent::SELF:
                this->processTouchMelee(pair.first, pair.second);
                break;
            default: Log("unhandled melee component type...");
                break;
        }
    }
}

void MeleeSystem::processTouchMelee(unsigned int eid, unsigned int oid)
{
    auto& cpMelee = ecs::get<cp::Melee>(eid);
    
    cpMelee.enabled = false;
    auto& cpInput2 = ecs::get<cp::Input>(oid);
    auto& cpRender = ecs::get<cp::Render>(eid);
    auto& cpRender2 = ecs::get<cp::Render>(oid);
    
    auto bounds1 = SysHelper::getBounds(eid);
    auto bounds2 = SysHelper::getBounds(oid);
    cc::Vec2 moveDir = { (bounds2.getMidX() - bounds1.getMidX()) / 2,
                         (bounds2.getMidY() - bounds1.getMidY()) / 2 };
    
    cpInput2.disable(1.0);
    context->ecs->add<cp::Untargetable>(oid);
    cpRender2.sprite->runAction(cc::Sequence::create(
        CocosHelper::blinkActionCreate({255,50,50}, def::blinkAnim::count / def::blinkAnim::duration, 1.0),
        cc::CallFunc::create([oid, this](){
            context->ecs->del<cp::Untargetable>(oid);
        }),
        NULL));
    
    auto resolutionAnim = cc::Sequence::create(
        cc::CallFunc::create([this, &cpRender2, &cpMelee, oid, moveDir](){
            /*ecs::get<cp::Physics>(oid).addForce(cpMelee.recoil.speed,
                                                cpMelee.recoil.duration,
                                                moveDir);*/
            cpMelee.enabled = true;
            ecs::get<cp::Health>(oid).damage += cpMelee.damage;
        }),
        NULL
    );
    
    cpRender2.sprite->runAction(
        CocosHelper::blinkActionCreate({100,255,255}, def::blinkAnim::count, def::blinkAnim::duration)
    );
    cpRender.sprite->runAction(resolutionAnim);
}

void MeleeSystem::processDirMelee(unsigned eid, unsigned oid, Dir atkDir)
{
    if (atkDir == Dir::None)
        return;

    auto& cpMelee = ecs::get<cp::Melee>(eid);
 
    cpMelee.enabled = false;
    auto& cpRender = ecs::get<cp::Render>(eid);
    auto& cpRender2 = ecs::get<cp::Render>(oid);
    
    auto bounds1 = SysHelper::getBounds(eid);
    auto bounds2 = SysHelper::getBounds(oid);
    cc::Vec2 moveDir = { (bounds2.getMidX() - bounds1.getMidX()) / 2,
                         (bounds2.getMidY() - bounds1.getMidY()) / 2 };
    
    context->ecs->del<cp::Position>(eid);
    
    auto animName = cpMelee.animKey.Value + ProfileData::getTagName(atkDir);
    auto animDuration = cpRender.getAnimationDuration(animName);
    
    ecs::get<cp::Input>(eid).disable(animDuration + def::blinkAnim::duration);
    ecs::get<cp::Input>(oid).disable(animDuration * cpMelee.triggerRatio +
                                     def::blinkAnim::duration + 0.1);
    
    auto moveAnim = cc::Sequence::create(
        cc::MoveBy::create(animDuration / 3, moveDir),
        cc::DelayTime::create(animDuration / 3),
        cc::MoveBy::create(animDuration / 3, moveDir)->reverse(),
        NULL
    );
    
    auto attackAnim = cc::CallFunc::create([&cpMelee, eid, this, &cpRender, atkDir](){
        auto animName = cpMelee.animKey.Value + ProfileData::getTagName(atkDir);
        cpRender.setAnimation(animName, 1, [&cpMelee, &cpRender, eid, this](bool canceled){
            cpMelee.enabled = true;
            this->context->ecs->add<cp::Position>(eid).set(cpRender.sprite->getPosition());
        });
    });
    
    auto resolutionAnim = cc::Sequence::create(
        cc::DelayTime::create(animDuration * cpMelee.triggerRatio),
        cc::CallFunc::create([this, &cpRender2, &cpMelee, oid, moveDir](){
            auto& cpHealth2 = ecs::get<cp::Health>(oid);
        
            auto blinkAction = CocosHelper::blinkActionCreate(
                {255,50,50}, def::blinkAnim::count / def::blinkAnim::duration, 1.0);
            blinkAction->setTag(def::blinkAnim::tag);
            cpRender2.sprite->runAction(blinkAction);
            /*ecs::get<cp::Physics>(oid).addForce(cpMelee.recoil.speed,
                                                cpMelee.recoil.duration,
                                                moveDir);
            ecs::get<cp::Input>(oid).disable("velocity", [this](unsigned eid) {
                if (ecs::get<cp::Physics>(eid).velocity == cc::Vec2::ZERO)
                {
                    context->ecs->del<cp::Untargetable>(eid);
                    ecs::get<cp::Render>(eid).sprite->stopAllActionsByTag(def::blinkAnim::tag);
                    ecs::get<cp::Render>(eid).sprite->setColor({255,255,255});
                    return false;
                }
                return true;
            });*/
        
            cpHealth2.damage += cpMelee.damage;
        }),
        NULL
    );
    
    cpRender2.sprite->runAction(
        CocosHelper::blinkActionCreate({100,255,255}, def::blinkAnim::count, def::blinkAnim::duration)
    );
    cpRender.sprite->runAction(cc::Sequence::create(
        cc::CallFunc::create([eid, oid, this](){
            context->ecs->add<cp::Untargetable>(oid) = true;
            context->ecs->add<cp::Untargetable>(eid) = true; }),
        CocosHelper::blinkActionCreate({50,50,255}, def::blinkAnim::count, def::blinkAnim::duration),
        cc::Spawn::create(
            moveAnim,
            attackAnim,
            resolutionAnim,
            NULL),
        cc::CallFunc::create([eid, this](){
            context->ecs->del<cp::Untargetable>(eid); }),
        NULL)
    );
}

Dir MeleeSystem::getAtkDir(unsigned int eid, const MeleeComponent &cpMelee)
{
    Dir atkDir = Dir::None;
    
    atkDir = ecs::get<cp::Orientation>(eid).dir;
        
    switch(cpMelee.type)
    {
        case MeleeComponent::CHARGE:
        case MeleeComponent::SELF: {
            atkDir = Dir::None;
            break;
        }
        case MeleeComponent::DIR: {
            atkDir = ecs::get<cp::Orientation>(eid).dir;
            break;
        }
        default:
            Log("melee type not implemented");
            break;
    }
    
    return atkDir;
}

cocos2d::Rect MeleeSystem::getAtkRectFromDir(const cocos2d::Rect& bounds,
                                             cc::Size range, const Dir& dir)
{
    switch(dir.getRaw())
    {
        case Dir::Left:
            return cc::Rect(bounds.getMinX() - range.width, bounds.getMidY() - range.height / 2, range.width, range.height);
        case Dir::Right:
            return cc::Rect(bounds.getMaxX(), bounds.getMidY() - range.height / 2, range.width, range.height);
        case Dir::Down:
            return cc::Rect(bounds.getMidX() - range.height / 2, bounds.getMinY() - range.width, range.height, range.width);
        case Dir::Up:
            return cocos2d::Rect(bounds.getMidX() - range.height / 2, bounds.getMaxY(), range.height, range.width);
        case Dir::None:
            return cc::Rect(bounds.getMinX() - range.width,
                            bounds.getMinY() - range.height,
                            bounds.size.width + range.width * 2,
                            bounds.size.height + range.height * 2);
        default:
            return cc::Rect::ZERO;
    }
}