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
        
            if (cpInput.orientation != Dir::kNone)
            {
                cpOrientation.lastDir = cpOrientation.curDir;
            
                if (cpOrientation.lastDir != Dir::kNone &&
                  ((cpOrientation.lastDir & cpInput.orientation) == cpOrientation.lastDir))
                    cpOrientation.curDir = cpOrientation.lastDir;
                else
                    cpOrientation.curDir = cpInput.orientation;
            }
        }
        
        //set velocity dir
        if (ecs::has<cp::Velocity>(eid))
        {
            auto& cpVelocity = ecs::get<cp::Velocity>(eid);
            if ((cpInput.orientation & cpInput.lastOrientation) == 0)
                cpVelocity.accelFactor = 0.0f;
            cpVelocity.direction = lib::getVectorDirection(cpInput.orientation);
        }
    }
}