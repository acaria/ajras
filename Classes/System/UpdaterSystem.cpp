#include "UpdaterSystem.h"
#include "Components.h"
#include "IMapData.h"
#include "SysHelper.h"
#include "CmdFactory.h"

void UpdaterSystem::tick(double dt)
{
    //update from inputs
    for(auto eid : context->ecs->join<cp::Input, cp::Position, cp::Physics>())
    {
        auto& cpInput = ecs::get<cp::Input>(eid);
        auto& cpPhy = ecs::get<cp::Physics>(eid);
        cpInput.updatePredicates(eid, dt);
        
        if (!cpInput.isActive()) //inhibitor
        {
            cpPhy.movement.direction = cc::Vec2::ZERO;
            cpPhy.movement.active = false;
            continue;
        }
        
        auto& cpPosition = ecs::get<cp::Position>(eid);
        
        if (cpInput.goTo != nullptr)
        {
            cpPhy.movement.active = true;
            auto bounds = SysHelper::getBounds(cpPosition, cpPhy);
            auto wayPoints = context->data->getNav()->getWaypoints(
                {bounds.getMidX(), bounds.getMidY()}, cpInput.goTo.Value, cpPhy.category);
            CmdFactory::goTo(context->ecs, eid, wayPoints, 2);
            
            cpInput.goTo = nullptr;
        }
        else
        {
            auto direction = cpInput.direction;
            if (direction.isZero())
                cpPhy.movement.active = false;
            else
            {
                cpPhy.movement.active = true;
                cpPhy.movement.direction = direction;
            }
            auto newDir = Dir::cardinalFromVec(direction);
            if (newDir != Dir::None)
            {
                if (cpPosition.dir == Dir::None)
                    cpPosition.dir = newDir;
                else if (!newDir.contains(cpPosition.dir))
                    cpPosition.dir = newDir;
            }
        }
    }
    
    //update stamina
    for(auto eid : context->ecs->join<cp::Stamina, cp::Input>())
    {
        if (!ecs::get<cp::Input>(eid).isActive())
            continue; //skip disabled
        auto& cpStamina = ecs::get<cp::Stamina>(eid);
        if (cpStamina.current < cpStamina.max)
        {
            cpStamina.current += cpStamina.recovery * dt;
            if (cpStamina.current > cpStamina.max)
                cpStamina.current = cpStamina.max;
        }
    }
    
    //update commands
    for(auto eid : context->ecs->system<cp::Cmd>())
    {
        ecs::get<cp::Cmd>(eid).process(eid, dt);
    }
}