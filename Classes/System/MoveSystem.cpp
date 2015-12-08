#include "MoveSystem.h"
#include "Components.h"
#include "IMapData.h"

void MoveSystem::tick(double dt)
{
    for(auto eid : context->ecs->join<cp::Physics, cp::Position>())
    {
        //shortcuts
        auto& cpPos = ecs::get<cp::Position>(eid);
        auto& cpPhy = ecs::get<cp::Physics>(eid);
    
        //save previous pos
        cpPos.lastPos = cpPos.pos;
    
        auto velocity = cc::Vec2::ZERO;
    
        //apply forces
        auto maxSpeed = 0;
        for(auto& pair : cpPhy.forces)
        {
            auto& force = pair.second;
            if (!force.active && force.curSpeed == 0)
                continue;
            if (force.active)
            {
                //maxSpeed = (maxSpeed == 0) ? force.maxSpeed : MIN(maxSpeed, force.maxSpeed);
                maxSpeed = MAX(maxSpeed, force.maxSpeed);
                force.curSpeed += force.accSpeed * dt;
            }
            else
            {
                force.curSpeed -= force.decSpeed * dt;
            }
            force.curSpeed = lib::clamp(force.curSpeed, 0.0f, force.maxSpeed);
            assert(force.direction.getLength() <= 1.01);
            velocity += force.direction * force.curSpeed * dt;
            
            if (force.duration != -1)
            {
                if (force.duration > 0)
                    force.duration = MAX(0, force.duration - dt);
                force.active = (force.duration > 0);
            }
        }
        
        //cap velocity
        if (maxSpeed != 0 && velocity.length() > maxSpeed)
            velocity = velocity.getNormalized() * maxSpeed;
        
        cpPhy.velocity = velocity;
        
        //update positions
        if (!cpPhy.velocity.isZero())
        {
            cpPos.pos += cpPhy.velocity;
            dispatcher->onEntityPositionChanged(context->ecs->getID(), eid);
        }
    }
}