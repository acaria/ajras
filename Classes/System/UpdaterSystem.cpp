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
        
        if (ecs::has<cp::Physics>(eid))
        {
            auto& cpPh = ecs::get<cp::Physics>(eid);
            cpPh.move.direction = cpInput.direction.getNormalized();
            auto newDir = Dir::fromVec(cpInput.direction);
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