#include "UpdaterSystem.h"
#include "Components.h"
#include "IMapData.h"
#include "SysHelper.h"

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
            auto target = cpInput.goTo.Value;
            ecs.add<cp::Cmd>(eid).set([target](unsigned eid){
                auto& cpPosition = ecs::get<cp::Position>(eid);
                auto& cpPhy = ecs::get<cp::Physics>(eid);
                auto bounds = SysHelper::getBounds(cpPosition, cpPhy);
                cc::Vec2 dir = target - cc::Vec2(bounds.getMidX(), bounds.getMidY());
                cpPhy.move.direction = dir.getNormalized();
                cpPosition.dir = Dir::fromVec(cpPhy.move.direction);
                if (dir.length() < 2)
                    return true;
                return false;
            });
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
    
    //command batcher
    auto cmds = ecs.system<cp::Cmd>();
    for(auto eid : cmds)
    {
        if (ecs::get<cp::Cmd>(eid).process(eid))
            ecs.del<cp::Cmd>(eid);
    }
}