#include "MoveSystem.h"
#include "Components.h"

void MoveSystem::tick(double dt)
{
    for(auto eid : ecs.join<cp::Physics, cp::Position>())
    {
        //shortcuts
        auto& cpPos = ecs::get<cp::Position>(eid);
        auto& cpPhy = ecs::get<cp::Physics>(eid);
    
        //save previous pos
        cpPos.lastPos = cpPos.pos;
    
        //compute velocity : priority force > move > inertia
        if (cpPhy.force.duration > 0) //force
        {
            cpPhy.force.duration -= dt;
            
            cpPhy.decelFactor = cpPhy.accelFactor;
            cpPhy.accelFactor = lib::clamp<float>((float)cpPhy.accelFactor + (dt / cpPhy.accelDuration), 0.0f, 1.0f);
            
            cpPhy.velocity = cpPhy.force.direction * cpPhy.getForceSpeed() * cpPhy.accelFactor * dt;
        }
        else if (!cpPhy.move.direction.isZero()) //move
        {
            cpPhy.decelFactor = cpPhy.accelFactor;
            cpPhy.accelFactor = lib::clamp<float>((float)cpPhy.accelFactor + (dt / cpPhy.accelDuration), 0.0f, 1.0f);
            
                
            cpPhy.velocity = cpPhy.move.direction * cpPhy.getMoveSpeed() * cpPhy.accelFactor * dt;
        }
        else //inertia
        {
            if (!cpPhy.velocity.isZero())
            {
                cpPhy.accelFactor = cpPhy.decelFactor;
                cpPhy.velocity.x *= cpPhy.decelFactor;
                cpPhy.velocity.y *= cpPhy.decelFactor;
                    
                cpPhy.decelFactor = lib::clamp<float>((float)cpPhy.decelFactor - (dt / cpPhy.decelDuration), 0.0f, 1.0f);
            }
        }
        
        //update positions
        if (!cpPhy.velocity.isZero())
        {
            cpPos.pos += cpPhy.velocity;
        }
    }
}