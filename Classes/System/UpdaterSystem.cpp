#include "UpdaterSystem.h"
#include "Components.h"

void UpdaterSystem::tick(double dt)
{
    //update from inputs
    for(auto eid : ecs.join<cp::Input, cp::Position>())
    {
        auto& cpInput = ecs::get<cp::Input>(eid);
        cpInput.updatePredicates(eid, dt);
        
        if (!cpInput.isActive()) //inhibitor
        {
            //reset velocity dir
            if (ecs::has<cp::Physics>(eid))
            {
                ecs::get<cp::Physics>(eid).move.direction = cc::Vec2::ZERO;
                ecs::get<cp::Physics>(eid).move.ratio = 1.0;
            }
            continue;
        }
        
        //set orientation
        auto& cpPosition = ecs::get<cp::Position>(eid);
        
        if (cpInput.orientation != Dir::None)
        {
            cpPosition.orientation = cpInput.exactOrientation;
            cpPosition.lastDir = cpPosition.curDir;
            
            if (cpPosition.lastDir != Dir::None && cpInput.orientation.contains(cpPosition.lastDir))
                cpPosition.curDir = cpPosition.lastDir;
            else
                cpPosition.curDir = cpInput.orientation;
        }
        
        //set velocity dir
        if (ecs::has<cp::Physics>(eid))
        {
            auto& cpVelocity = ecs::get<cp::Physics>(eid);
            if (cpInput.orientation.uncross(cpInput.lastOrientation))
            {
                cpInput.lastOrientation = cpInput.orientation;
                cpVelocity.accelFactor = 0.0f;
            }
            cpVelocity.move.direction = cpInput.direction;
        }
    }
    
    //update stamina
    for(auto eid : ecs.join<cp::Stamina, cp::Input>())
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
}