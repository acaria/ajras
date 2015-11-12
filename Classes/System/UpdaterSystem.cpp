#include "UpdaterSystem.h"
#include "Components.h"

void UpdaterSystem::tick(double dt)
{
    //update from inputs
    for(auto eid : ecs.system<cp::Input>())
    {
        auto& cpInput = ecs::get<cp::Input>(eid);
        cpInput.updatePredicates(eid, dt);
        
        if (!cpInput.isActive()) //inhibitor
        {
            //reset velocity dir
            if (ecs::has<cp::Velocity>(eid))
            {
                ecs::get<cp::Velocity>(eid).move.direction = cc::Vec2::ZERO;
                ecs::get<cp::Velocity>(eid).move.ratio = 1.0;
            }
            continue;
        }
        
        //set orientation
        if (ecs::has<cp::Orientation>(eid))
        {
            auto& cpOrientation = ecs::get<cp::Orientation>(eid);
        
            if (cpInput.orientation != Dir::None)
            {
                cpOrientation.visual = cpInput.exactOrientation;
                cpOrientation.lastDir = cpOrientation.curDir;
            
                if (cpOrientation.lastDir != Dir::None && cpInput.orientation.contains(cpOrientation.lastDir))
                    cpOrientation.curDir = cpOrientation.lastDir;
                else
                    cpOrientation.curDir = cpInput.orientation;
            }
        }
        
        //set velocity dir
        if (ecs::has<cp::Velocity>(eid))
        {
            auto& cpVelocity = ecs::get<cp::Velocity>(eid);
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