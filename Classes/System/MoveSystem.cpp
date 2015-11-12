#include "MoveSystem.h"
#include "Components.h"

void MoveSystem::tick(double dt)
{
    for(auto eid : ecs.join<cp::Velocity, cp::Position, cp::Input>())
    {
        //shortcuts
        auto& cpPos = ecs::get<cp::Position>(eid);
        auto& cpVel = ecs::get<cp::Velocity>(eid);
    
        //save previous pos
        cpPos.last = cpPos.pos;
    
        //compute velocity : priority force > move > inertia
        if (cpVel.force.duration > 0) //force
        {
            cpVel.force.duration -= dt;
            
            cpVel.decelFactor = cpVel.accelFactor;
            cpVel.accelFactor = lib::clamp<float>((float)cpVel.accelFactor + (dt / cpVel.accelDuration), 0.0f, 1.0f);
            
            cpVel.velocity = cpVel.force.direction * cpVel.getForceSpeed() * cpVel.accelFactor * dt;
        }
        else if (!cpVel.move.direction.isZero()) //move
        {
            cpVel.decelFactor = cpVel.accelFactor;
            cpVel.accelFactor = lib::clamp<float>((float)cpVel.accelFactor + (dt / cpVel.accelDuration), 0.0f, 1.0f);
            
                
            cpVel.velocity = cpVel.move.direction * cpVel.getMoveSpeed() * cpVel.accelFactor * dt;
        }
        else //inertia
        {
            if (!cpVel.velocity.isZero())
            {
                cpVel.accelFactor = cpVel.decelFactor;
                cpVel.velocity.x *= cpVel.decelFactor;
                cpVel.velocity.y *= cpVel.decelFactor;
                    
                cpVel.decelFactor = lib::clamp<float>((float)cpVel.decelFactor - (dt / cpVel.decelDuration), 0.0f, 1.0f);
            }
        }
        
        //update positions
        if (!cpVel.velocity.isZero())
        {
            cpPos.pos += cpVel.velocity;
        }
    }
}