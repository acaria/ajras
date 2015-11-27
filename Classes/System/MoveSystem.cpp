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
        
        if (!cpPhy.enabled) continue;
    
        //save previous pos
        cpPos.lastPos = cpPos.pos;
    
        auto velocity = cc::Vec2::ZERO;
    
        //compute movement velocity
        if (cpPhy.movement.active)
            cpPhy.movement.curSpeed += cpPhy.movement.accSpeed * dt;
        else
            cpPhy.movement.curSpeed -= cpPhy.movement.decSpeed * dt;
        cpPhy.movement.curSpeed = lib::clamp(cpPhy.movement.curSpeed,
                                             0.0f, cpPhy.movement.maxSpeed);
        velocity += cpPhy.movement.direction.getNormalized() * cpPhy.movement.curSpeed * dt;
        
        for(auto it = cpPhy.external.begin(); it != cpPhy.external.end();)
        {
            if (it->curSpeed == 0 && !it->active)
                it = cpPhy.external.erase(it);
            else
            {
                auto& force = *it;
                if (force.active)
                    force.curSpeed += force.accSpeed * dt;
                else
                    force.curSpeed -= force.decSpeed * dt;
                force.curSpeed = lib::clamp(force.curSpeed, 0.0f, force.maxSpeed);
                velocity += force.direction.getNormalized() * force.curSpeed * dt;
                
                if (force.duration > 0)
                {
                    force.duration -= dt;
                    force.active = (force.duration > 0);
                }
            }
        }
        
        cpPhy.velocity = velocity;
        
        /*
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
        }*/
        
        //update positions
        if (!cpPhy.velocity.isZero())
        {
            cpPos.pos += cpPhy.velocity;
            dispatcher->onEntityPositionChanged(context->ecs->getID(), eid);
        }
    }
}