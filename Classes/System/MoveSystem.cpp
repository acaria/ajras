#include "Headers.h"

void MoveSystem::tick(double dt)
{
    for(auto eid : ecs.join<cp::Velocity, cp::Position>())
    {
        //shortcuts
        auto& cpPos = ecs::get<cp::Position>(eid);
        auto& cpVel = ecs::get<cp::Velocity>(eid);
    
        //slowing target move
        float speed = cpVel.speed;
        if (ecs::has<cp::Target>(eid) && ecs::get<cp::Target>(eid) != 0)
            speed *= 0.6;
    
        //save previous pos
        cpPos.last = cpPos.pos;
        
        //compute velocity
        if (!cpVel.direction.isZero())
        {
            cpVel.accelFactor = lib::clamp<float>((float)cpVel.accelFactor + (dt / cpVel.accelDuration), 0.0f, 1.0f);
            cpVel.decelFactor = cpVel.accelFactor;
                
            cpVel.velocity.x = cpVel.direction.x * speed * cpVel.accelFactor * dt;
            cpVel.velocity.y = cpVel.direction.y * speed * cpVel.accelFactor * dt;
        }
        else
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