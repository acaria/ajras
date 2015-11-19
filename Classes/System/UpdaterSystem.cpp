#include "UpdaterSystem.h"
#include "Components.h"
#include "IMapData.h"
#include "SysHelper.h"
#include "CmdFactory.h"

void UpdaterSystem::init(IMapData* data)
{
    this->data = data;
}

void UpdaterSystem::tick(double dt)
{
    //update from inputs
    for(auto eid : ecs.join<cp::Input, cp::Position, cp::Physics>())
    {
        auto& cpInput = ecs::get<cp::Input>(eid);
        auto& cpPhy = ecs::get<cp::Physics>(eid);
        cpInput.updatePredicates(eid, dt);
        
        if (!cpInput.isActive()) //inhibitor
        {
            //reset velocity dir
            cpPhy.move.direction = cc::Vec2::ZERO;
            cpPhy.move.ratio = 1.0;
            continue;
        }
        
        auto& cpPosition = ecs::get<cp::Position>(eid);
        
        if (cpInput.goTo != nullptr)
        {
            auto bounds = SysHelper::getBounds(cpPosition, cpPhy);
            auto wayPoints = this->data->getNav()->getWaypoints(
                {bounds.getMidX(), bounds.getMidY()}, cpInput.goTo.Value, cpPhy.category);
            CmdFactory::goTo(this->ecs, eid, wayPoints, 2);
            
            cpInput.goTo = nullptr;
        }
        else
        {
            cpPhy.move.direction = cpInput.direction.getNormalized();
            auto newDir = Dir::fromVec(cpPhy.move.direction);
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
    
    //update commands
    for(auto eid : ecs.system<cp::Cmd>())
    {
        ecs::get<cp::Cmd>(eid).process(eid, dt);
    }
}