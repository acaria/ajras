#include "UpdaterSystem.h"
#include "Components.h"
#include "IMapData.h"
#include "CmdFactory.h"

void UpdaterSystem::tick(double dt)
{
    //clean removed entities
    for(auto eid : toRemove)
    {
        cp::entity::remove(eid, context->ecs->getID());
    }
    
    //update commands
    for(auto eid : context->ecs->system<cp::Cmd>())
    {
        ecs::get<cp::Cmd>(eid).process(eid, dt);
    }
    
    //update from inputs
    for(auto eid : context->ecs->join<cp::Input, cp::Position, cp::Physics>())
    {
        auto& cpInput = ecs::get<cp::Input>(eid);
        
        if (!cpInput.enabled) //inhibitor
            continue;
        
        auto& cpPhy = ecs::get<cp::Physics>(eid);
        
        auto& cpPosition = ecs::get<cp::Position>(eid);
        
        if (cpInput.goTo != nullptr)
        {
            auto bounds = SysHelper::getBounds(cpPosition, cpPhy);
            auto wayPoints = context->data->getNav()->getWaypoints(
                {bounds.getMidX(), bounds.getMidY()}, cpInput.goTo.Value, cpPhy.category);
            CmdFactory::goTo(context->ecs, eid, wayPoints, 2);
            
            cpInput.goTo = nullptr;
        }
        else
        {
            if (cpInput.direction.isZero())
                cpPhy.fInput().active = false;
            else
            {
                cpPhy.fInput().active = true;
                cpPhy.fInput().direction = cpInput.direction;
            }
            if (ecs::has<cp::Orientation>(eid))
            {
                auto& cpOrientation = ecs::get<cp::Orientation>(eid);
                auto newDir = Dir::cardinalFromVec(cpInput.direction);
                if (newDir != Dir::None)
                {
                    if (cpOrientation.dir == Dir::None)
                        cpOrientation.dir = newDir;
                    else if (!newDir.contains(cpOrientation.dir))
                        cpOrientation.dir = newDir;
                }
            }
        }
    }
    
    //update stamina
    for(auto eid : context->ecs->join<cp::Stamina, cp::Input>())
    {
        if (!ecs::get<cp::Input>(eid).enabled)
            continue; //skip disabled
        auto& cpStamina = ecs::get<cp::Stamina>(eid);
        if (cpStamina.current < cpStamina.max)
        {
            cpStamina.current += cpStamina.recovery * dt;
            if (cpStamina.current > cpStamina.max)
                cpStamina.current = cpStamina.max;
        }
    }
    
    //update health
    for(auto eid : context->ecs->join<cp::Health, cp::Render>())
    {
        auto& cpHealth = ecs::get<cp::Health>(eid);
        auto& cpRender = ecs::get<cp::Render>(eid);
        
        if (cpHealth.damage > 0)
        {
            cpHealth.hp -= cpHealth.damage;
            cpHealth.damage = 0;
            
            if (cpHealth.hp <= 0)
            {
                if (ecs::has<cp::Input>(eid))
                    ecs::get<cp::Input>(eid).enabled = false;
                cpRender.sprite->stopAllActions();
                cpRender.sprite->setColor({255,255,255});
                cpRender.setAnimation("death", 1, [eid, this](bool cancel){
                    this->toRemove.push_back(eid);
                    dispatcher->onEntityDeleted(context->ecs->getID(), eid);
                });
            }
        }
    }
}