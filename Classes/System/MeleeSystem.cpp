#include "MeleeSystem.h"
#include "Components.h"
#include "AnimationData.h"
#include "MissionScene.h"
#include "ControlSystem.h"
#include "GameCtrl.h"
#include "IMapData.h"
#include "CocosHelper.h"

void MeleeSystem::tick(double dt)
{
    std::list<std::list<std::pair<unsigned, unsigned>>> meleeListGroup;
    auto entityList = context->ecs->join<cp::Melee, cp::Mood, cp::Physics, cp::Position, cp::Render>();
    for(auto eid : entityList)
    {
        if (!ecs::has<cp::Input>(eid) || !ecs::get<cp::Input>(eid).enabled)
            continue;
        
        auto& cpMelee = ecs::get<cp::Melee>(eid);
        if (!cpMelee.enabled || cpMelee.type == MeleeComponent::NONE)
            continue;
        
        auto entityMood = ecs::get<cp::Mood>(eid);
        auto oppositeMoods = def::mood::getOpponents(entityMood);
        
        cc::Rect body = SysHelper::getBounds(eid);
        
        Dir atkDir = getAtkDir(eid, cpMelee);
        cpMelee.atkRect = getAtkRectFromDir(body, cpMelee.range, atkDir);

        if (cpMelee.atkRect.size.width == 0 || cpMelee.atkRect.size.height == 0)
            continue;
        
        //check room objects
        for(auto oid : entityList)
        {
            if (oid == eid) continue;
            
            //if (ecs::has<cp::Untargetable>(oid))
            //    continue;
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
    //load attacker components
    auto& cpRender = ecs::get<cp::Render>(eid);
    auto& cpMelee = ecs::get<cp::Melee>(eid);
    //load target components
    auto& cpRender2 = ecs::get<cp::Render>(oid);
    
    //disable both entities attacker + target
    this->setEntityAvailability(eid, false);
    this->setEntityAvailability(oid, false);
    
    //define attack dir
    auto bounds1 = SysHelper::getBounds(eid);
    auto bounds2 = SysHelper::getBounds(oid);
    cc::Vec2 moveDir = { (bounds2.getMidX() - bounds1.getMidX()) / 2,
                         (bounds2.getMidY() - bounds1.getMidY()) / 2 };
    
    //target: resolution anim
    auto resolutionAnim = cc::Sequence::create(
        cc::CallFunc::create([this, &cpMelee, eid, oid, moveDir](){
            this->setEntityAvailability(eid, true);
            this->setEntityAvailability(oid, true);
        
            //apply impact force to target
            auto& cpPhy2 = ecs::get<cp::Physics>(oid);
            cpPhy2.fImpact().curSpeed = 0;
            cpPhy2.fImpact().accSpeed = 600;
            cpPhy2.fImpact().decSpeed = 250;
            cpPhy2.fImpact().maxSpeed = 150;
            cpPhy2.fImpact().direction = moveDir.getNormalized();
            cpPhy2.fImpact().active = true;
            cpPhy2.fImpact().duration = 0.15;
        
            ecs::get<cp::Health>(oid).damage += cpMelee.damage;
        }),
        NULL
    );
    
    //run actions
    cpRender2.sprite->runAction(
        CocosHelper::blinkActionCreate({255,50,50}, blinkAnim::count, blinkAnim::duration)
    );

    cpRender.sprite->runAction(resolutionAnim);
}

void MeleeSystem::processDirMelee(unsigned eid, unsigned oid, Dir atkDir)
{
    if (atkDir == Dir::None)
        return;

    //load attacker components
    auto& cpMelee = ecs::get<cp::Melee>(eid);
    auto& cpRender = ecs::get<cp::Render>(eid);
    //load target components
    auto& cpRender2 = ecs::get<cp::Render>(oid);
    
    //define attack dir
    auto bounds1 = SysHelper::getBounds(eid);
    auto bounds2 = SysHelper::getBounds(oid);
    cc::Vec2 moveDir = { (bounds2.getMidX() - bounds1.getMidX()) / 2,
        (bounds2.getMidY() - bounds1.getMidY()) / 2 };
    
    //define melee anim
    auto animName = cpMelee.animKey.Value + ProfileData::getTagName(atkDir);
    auto animDuration = cpRender.getAnimationDuration(animName);
    
    //disable both entities attacker + target
    this->setEntityAvailability(eid, false);
    this->setEntityAvailability(oid, false);
    
    //exclude attacker from systems
    context->ecs->del<cp::Position>(eid);
    dispatcher->onEntityPositionChanged(context->ecs->getID(), eid);
    
    //attacker: estocade anim
    auto prepareAnim = cc::Sequence::create(
        cc::MoveBy::create(animDuration / 3, moveDir),
        cc::DelayTime::create(animDuration / 3),
        cc::MoveBy::create(animDuration / 3, moveDir)->reverse(),
        NULL
    );
    
    //attacker: attack anim
    auto attackAnim = cc::CallFunc::create([&cpMelee, eid, this, &cpRender, atkDir](){
        auto animName = cpMelee.animKey.Value + ProfileData::getTagName(atkDir);
        cpRender.setAnimation(animName, 1, [&cpRender, eid, this](bool canceled){
            context->ecs->add<cp::Position>(eid).set(cpRender.sprite->getPosition());
            dispatcher->onEntityPositionChanged(context->ecs->getID(), eid);
            setEntityAvailability(eid, true);
        });
    });
    
    //target: resolution anim (success, TODO:fail)
    auto resolutionAnim = cc::Sequence::create(
        cc::DelayTime::create(animDuration * cpMelee.triggerRatio),
        cc::CallFunc::create([this, eid, oid, moveDir](){
            //attacker
            auto& cpMelee = ecs::get<cp::Melee>(eid);
            //target
            auto& cpRender2 = ecs::get<cp::Render>(oid);
            auto& cpPhy2 =  ecs::get<cp::Physics>(oid);
            auto& cpHealth2 = ecs::get<cp::Health>(oid);
        
            this->setEntityAvailability(oid, true);
        
            //blink target
            auto blinkAction = CocosHelper::blinkActionCreate(
                {255,50,50}, blinkAnim::count / blinkAnim::duration, 1.0);
            blinkAction->setTag(blinkAnim::tag);
            cpRender2.sprite->runAction(blinkAction);
        
            //apply impact force to target
            cpPhy2.fImpact().curSpeed = 0;
            cpPhy2.fImpact().accSpeed = 800;
            cpPhy2.fImpact().decSpeed = 250;
            cpPhy2.fImpact().maxSpeed = 200;
            cpPhy2.fImpact().direction = moveDir.getNormalized();
            cpPhy2.fImpact().active = true;
            cpPhy2.fImpact().duration = 0.12;
        
            cpHealth2.damage += cpMelee.damage;
        }),
        NULL
    );
    
    //run actions
    cpRender2.sprite->runAction(
        CocosHelper::blinkActionCreate({100,255,255}, blinkAnim::count, blinkAnim::duration)
    );
    cpRender.sprite->runAction(cc::Sequence::create(
        CocosHelper::blinkActionCreate({50,50,255}, blinkAnim::count, blinkAnim::duration),
        cc::Spawn::create(
            prepareAnim,
            attackAnim,
            resolutionAnim,
            NULL),
        NULL)
    );
}

void MeleeSystem::setEntityAvailability(unsigned int eid, bool enabled)
{
    if (!enabled)
    {
        ecs::get<cp::Physics>(eid).resetForces();
    }
    ecs::get<cp::Melee>(eid).enabled = enabled;
    ecs::get<cp::Input>(eid).enabled = enabled;
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
            if (ecs::has<cp::Orientation>(eid))
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
            return cc::Rect(bounds.getMidX() - range.height / 2, bounds.getMaxY(), range.height, range.width);
        case Dir::None:
            return cc::Rect(bounds.getMinX() - range.width,
                            bounds.getMinY() - range.height,
                            bounds.size.width + range.width * 2,
                            bounds.size.height + range.height * 2);
        default:
            return cc::Rect::ZERO;
    }
}