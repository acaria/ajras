#include "CmdFactory.h"
#include "SysHelper.h"

using State = CmdComponent::State;

std::string CmdFactory::goTo(lib::EcsGroup* ecs, unsigned eid, cc::Vec2 target, float nearDistance)
{
    std::string tagName = "goto";
    ecs->add<cp::Cmd>(eid).set(tagName, [target, nearDistance](unsigned eid, double dt){
        if (!ecs::has<cp::Position, cp::Physics>(eid))
            return State::failure;
        
        auto& cpPosition = ecs::get<cp::Position>(eid);
        auto& cpPhy = ecs::get<cp::Physics>(eid);
        auto bounds = SysHelper::getBounds(cpPosition, cpPhy);
        cc::Vec2 dir = target - cc::Vec2(bounds.getMidX(), bounds.getMidY());
        
        cpPhy.fInput().active = true;
        cpPhy.fInput().direction = dir.getNormalized();
        
        if (ecs::has<cp::Orientation>(eid))
            ecs::get<cp::Orientation>(eid).set(Dir::cardinalFromVec(cpPhy.fInput().direction));

        if (dir.length() < nearDistance)
            return State::success;
        return State::inProgress;
    });
    return tagName;
}

std::string CmdFactory::goTo(lib::EcsGroup* ecs,
                             unsigned eid,
                             std::list<cc::Vec2> waypoints,
                             float nearDistance)
{
    std::string tagName = "goto";
    ecs->add<cp::Cmd>(eid).set(tagName, [waypoints, nearDistance](unsigned eid, double dt)mutable{
        if (!ecs::has<cp::Position, cp::Physics, cp::Input>(eid))
            return State::failure;
        
        if (waypoints.size() == 0)
            return State::success;
        auto target = waypoints.front();
        auto& cpPosition = ecs::get<cp::Position>(eid);
        auto& cpPhy = ecs::get<cp::Physics>(eid);
        auto bounds = SysHelper::getBounds(cpPosition, cpPhy);
        cc::Vec2 dir = target - cc::Vec2(bounds.getMidX(), bounds.getMidY());
        //hack for decelerations
        ecs::get<cp::Input>(eid).direction = dir.getNormalized() * MIN(1.0, dir.length() / 5);
        
        if (dir.length() < nearDistance)
            waypoints.pop_front();
        return State::inProgress;
    });
    return tagName;
}
