#include "MeleeSystem.h"
#include "Components.h"
#include "AnimationData.h"
#include "MissionScene.h"
#include "ControlSystem.h"
#include "SysHelper.h"
#include "GameCtrl.h"

void MeleeSystem::tick(double dt)
{
    std::list<std::list<std::pair<unsigned, unsigned>>> meleeListGroup;
    for(auto eid : ecs.join<cp::Melee, cp::Mood, cp::Stamina, cp::Collision, cp::Position, cp::Render>())
    {
        if (!ecs::has<cp::Input>(eid) || ecs::get<cp::Input>(eid).disabled)
            continue;
        
        auto entityMood = ecs::get<cp::Mood>(eid);
        auto oppositeMoods = def::mood::getOpponents(entityMood);
        
        auto& cpMelee = ecs::get<cp::Melee>(eid);
        if (cpMelee.processing)
            continue;

        cocos2d::Rect body = SysHelper::getBounds(eid);
        
        Dir atkDir = getAtkDir(eid, cpMelee);
        cpMelee.atkRect = getAtkRectFromDir(body, cpMelee.range, atkDir);

        if (cpMelee.atkRect.equals(cc::Rect::ZERO))
            continue;
        
        //check room objects
        for(auto oid : ecs.join<cp::Render, cp::Collision, cp::Position, cp::Health, cp::Mood, cp::Input>())
        {
            if (oid == eid) continue;
            
            if ((!ecs::has<cp::Target>(eid) || ecs::get<cp::Target>(eid) == oid) &&
                def::mood::inside(ecs::get<cp::Mood>(oid), oppositeMoods))
            {
                cocos2d::Rect bounds = SysHelper::getBounds(oid);
                
                if (cpMelee.atkRect.intersectsRect(bounds) && !cpMelee.processing &&
                    ecs::get<cp::Stamina>(eid).current >= cpMelee.energie)
                {
                    bool added = false;
                    for(auto& meleeList : meleeListGroup)
                    {
                        for(auto pair : meleeList)
                        {
                            if (pair.first == oid && pair.second == eid)
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
                if (cpStamina.current > best)
                {
                    best = cpStamina.current;
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
        cpMelee.processing = true;
        ecs::get<cp::Stamina>(pair.first).current -= cpMelee.energie;
        this->processMelee(pair.first, pair.second, getAtkDir(pair.first, cpMelee));
    }
}

void MeleeSystem::processMelee(unsigned eid, unsigned oid, Dir atkDir)
{
    auto& cpMelee = ecs::get<cp::Melee>(eid);
    auto& cpInput = ecs::get<cp::Input>(eid);
    auto& cpInput2 = ecs::get<cp::Input>(oid);
    auto& cpRender = ecs::get<cp::Render>(eid);
    auto& cpRender2 = ecs::get<cp::Render>(oid);
    
    auto bounds1 = SysHelper::getBounds(eid);
    auto bounds2 = SysHelper::getBounds(oid);
    cc::Vec2 moveDir = { (bounds2.getMidX() - bounds1.getMidX()) / 2,
                         (bounds2.getMidY() - bounds1.getMidY()) / 2 };
    
    cpRender.manualPosMode = true;
    
    auto animName = cpMelee.name + ProfileData::getTagName(atkDir);
    auto animDuration = cpRender.getAnimationDuration(animName);
    
    cpInput.disable(animDuration + def::blinkAnim::duration);
    cpInput2.disable(animDuration + def::blinkAnim::duration);
    
    auto df = def::blinkAnim::duration / def::blinkAnim::count / 2;
    auto redBlinkAnim = cc::Repeat::create(
        cc::Sequence::create(
            cc::TintTo::create(df, 255, 50, 50),
            cc::TintTo::create(df, 255, 255, 255),
            NULL),
        def::blinkAnim::count);
    
    auto blueBlinkAnim = cc::Repeat::create(
        cc::Sequence::create(
            cc::TintTo::create(df, 50, 50, 255),
            cc::TintTo::create(df, 255, 255, 255),
            NULL),
        def::blinkAnim::count);
    
    auto moveAnim = cc::Sequence::create(
        cc::MoveBy::create(animDuration / 3, moveDir),
        cc::DelayTime::create(animDuration / 3),
        cc::MoveBy::create(animDuration / 3, moveDir)->reverse(),
        NULL
    );
    
    auto attackAnim = cc::CallFunc::create([&cpMelee, &cpRender, atkDir](){
        auto animName = cpMelee.name + ProfileData::getTagName(atkDir);
        cpRender.setAnimation(animName, 1, [&cpMelee, &cpRender](bool canceled){
            cpMelee.processing = false;
            cpRender.manualPosMode = false;
        });
    });
    
    auto resolutionAnim = cc::Sequence::create(
        cc::DelayTime::create(animDuration / 2),
        cc::CallFunc::create([this, &cpRender2, &cpMelee, oid, moveDir](){
            auto& cpHealth2 = ecs::get<cp::Health>(oid);
            if (ecs::has<cp::Velocity>(oid))
            {
                ecs::get<cp::Velocity>(oid).decelFactor = 1;
                ecs::get<cp::Velocity>(oid).velocity = moveDir.getNormalized() * 2;
            }
        
            cpHealth2.hp -= cpMelee.damage;
            this->onHealthChanged(oid, cpHealth2.hp);
        
            if (cpHealth2.hp == 0)
            {
                ecs::get<cp::Input>(oid).forceDisable();
                //ecs.del<cp::Input>(oid);
                cpRender2.setAnimation("death", 1, [oid, this](bool cancel){
                    cp::entity::remove(oid, ecs.getID());
                });
            }
        }),
        NULL
    );
    
    cpRender2.sprite->runAction(redBlinkAnim);
    cpRender.sprite->runAction(cc::Sequence::create(
        blueBlinkAnim,
        cc::Spawn::create(
            resolutionAnim,
            moveAnim,
            attackAnim,
            NULL),
        NULL)
    );
}

bool MeleeSystem::detectStriking(unsigned int eid)
{
    switch(ecs::get<cp::Melee>(eid).type)
    {
        case MeleeComponent::SELF:
            return true;
        case MeleeComponent::DIR: {
            auto& cpRender = ecs::get<cp::Render>(eid);
            auto anim = cpRender.getCurAnim();
            auto p = cpRender.elapsedTime / anim->duration();
            if (p > 0.3 && p < 0.7)
                return true;
        }
        default: return false;
    }
    return false;
}

Dir MeleeSystem::getAtkDir(unsigned int eid, const MeleeComponent &cpMelee)
{
    Dir atkDir = Dir::None;
    
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
    
    return atkDir;
}

cocos2d::Rect MeleeSystem::getAtkRectFromDir(const cocos2d::Rect& bounds, cc::Size range, const Dir& dir)
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