#include "CmdFactory.h"
#include "SysHelper.h"

using State = CmdComponent::State;

void CmdFactory::goTo(lib::EcsGroup* ecs, unsigned eid, cc::Vec2 target, float nearDistance)
{
    ecs->add<cp::Cmd>(eid).set("goto", [target, nearDistance](unsigned eid, double dt){
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
}

void CmdFactory::goTo(lib::EcsGroup* ecs,
                      unsigned eid,
                      std::list<cc::Vec2> waypoints,
                      float nearDistance)
{
    ecs->add<cp::Cmd>(eid).set("goto", [waypoints, nearDistance](unsigned eid, double dt) mutable {
        if (!ecs::has<cp::Position, cp::Physics, cp::Input>(eid))
            return State::failure;
        
        if (waypoints.size() == 0)
            return State::success;
#if ECSYSTEM_DEBUG
        ecs::get<cp::Input>(eid).wayPoints = waypoints;
#endif
        auto target = waypoints.front();
        auto& cpPosition = ecs::get<cp::Position>(eid);
        auto& cpPhy = ecs::get<cp::Physics>(eid);
        auto bounds = SysHelper::getBounds(cpPosition, cpPhy);
        cc::Vec2 dir = target - cc::Vec2(bounds.getMidX(), bounds.getMidY());
        //hack for decelerations
        ecs::get<cp::Input>(eid).direction = dir.getNormalized() * MIN(1.0, dir.length() / 5);
        
        if (dir.length() < nearDistance)
        {
            waypoints.pop_front();
#if ECSYSTEM_DEBUG
            ecs::get<cp::Input>(eid).wayPoints.pop_front();
#endif
        }
        return State::inProgress;
    });
}

void CmdFactory::delay(lib::EcsGroup* ecs, unsigned int eid,
                       double timeInterval, const std::function<void()>& success)
{
    static long id = 1;
    ecs->add<cp::Cmd>(eid).set(std::to_string(id++),
            [timeInterval](unsigned eid, double dt) mutable {
        if (timeInterval <= 0)
            return State::success;
        timeInterval -= dt;
        return State::inProgress;
    }, success);
}