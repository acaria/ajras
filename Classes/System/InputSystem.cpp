#include "Headers.h"

bool InputSystem::checkPredicates(unsigned eid, InputComponent &cp)
{
    if (cp.predicates.size() == 0)
        return true;
    for(auto check : cp.predicates)
    {
        if (!check(eid))
            return false;
    }
    cp.predicates.clear();
    return true;
}

void InputSystem::tick(double dt)
{
    for(auto eid : ecs.system<cp::Input>())
    {
        auto& cpInput = ecs::get<cp::Input>(eid);
        
        if (!checkPredicates(eid, cpInput)) //inhibitor
        {
            //reset velocity dir
            if (ecs::has<cp::Velocity>(eid))
                ecs::get<cp::Velocity>(eid).direction = Vec2::ZERO;
            cpInput.disabled = true;
            continue;
        }
        cpInput.disabled = false;
        
        //set orientation
        if (ecs::has<cp::Orientation>(eid))
        {
            auto& cpOrientation = ecs::get<cp::Orientation>(eid);
        
            if (cpInput.orientation != Dir::None)
            {
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
            cpVelocity.direction = cpInput.direction;
        }
    }
}