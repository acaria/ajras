#include "CmdFactory.h"
#include "SysHelper.h"
#include "GameCtrl.h"
#include "ColorUtil.h"

long CmdFactory::tagUID = 1;

CmdFactory CmdFactory::at(lib::EcsGroup* ecs, unsigned eid,
        const std::function<void()>& onSuccess, const std::function<void()>& onFailure)
{
    return CmdFactory(ecs->getID(), eid, onSuccess, onFailure);
}

CmdFactory CmdFactory::at(unsigned gid, unsigned eid,
        const std::function<void()>& onSuccess, const std::function<void()>& onFailure)
{
    return CmdFactory(gid, eid, onSuccess, onFailure);
}

void CmdFactory::goTo(cc::Vec2 target, float nearDistance)
{
    ecs::add<cp::Cmd>(eid, gid).setTick("goto",
            [target, nearDistance](unsigned eid, double dt){
        if (!ecs::has<cp::Input, cp::Position, cp::Physics>(eid))
            return State::failure;

        auto bounds = SysHelper::getBounds(eid);
        cc::Vec2 dir = target - cc::Vec2(bounds.getMidX(), bounds.getMidY());
        
        ecs::get<cp::Input>(eid).direction = dir.getNormalized();

        if (dir.length() < nearDistance)
        {
            ecs::get<cp::Input>(eid).direction = {0,0};
            return State::success;
        }
        return State::inProgress;
    }, onSuccess, onFailure);
}

void CmdFactory::goBy(cc::Vec2 dir)
{
    if (!ecs::has<cp::Position>(eid))
    {
        if (onFailure != nullptr) onFailure();
        return;
    }
    auto origin = ecs::get<cp::Position>(eid).pos;
    auto length = dir.getLengthSq();
    ecs::add<cp::Cmd>(eid, gid).setTick("goby",
            [origin, dir, length](unsigned eid, double dt) {
        if (!ecs::has<cp::Input, cp::Position, cp::Physics>(eid))
            return State::failure;
                
        auto bounds = SysHelper::getBounds(eid);
                                            
        ecs::get<cp::Input>(eid).direction = dir.getNormalized();
        auto currentLength = (ecs::get<cp::Position>(eid).pos - origin).getLengthSq();
        if (currentLength >= length)
        {
            ecs::get<cp::Input>(eid).direction = {0,0};
            return State::success;
        }
        return State::inProgress;
    }, onSuccess, onFailure);
}

void CmdFactory::goTo(std::list<cc::Vec2> waypoints, float nearDistance)
{
    ecs::add<cp::Cmd>(eid, gid).setTick("goto",
            [waypoints, nearDistance](unsigned eid, double dt) mutable {
        if (!ecs::has<cp::Position, cp::Physics, cp::Input>(eid))
            return State::failure;

        if (waypoints.size() == 0)
        {
            ecs::get<cp::Input>(eid).direction = {0,0};
            return State::success;
        }
        
        if (!ecs::get<cp::Input>(eid).enabled)
            return State::failure;
            
        ecs::get<cp::Debug>(eid).wayPoints = waypoints;

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
            ecs::get<cp::Debug>(eid).wayPoints = waypoints;
        }
        return State::inProgress;
    }, onSuccess, onFailure);
}

void CmdFactory::animParamBy(const std::string &tag, float &current, float change, float duration)
{
    return animParamTo(tag, current, current + change, duration);
}

void CmdFactory::animParamBy(const std::string &tag,
                             std::list<std::reference_wrapper<float>> current,
                             std::list<float> changeBy, float duration)
{
    auto it1 = current.begin();
    auto it2 = changeBy.begin();
    std::list<float> to;
    
    while(it1 != current.end() && it2 != changeBy.end())
        to.push_back(*it1++ + *it2++);

    return animParamTo(tag, current, to, duration);
}

void CmdFactory::animParamTo(const std::string &tag, float &current, float target, float duration)
{
    if (current == target)
    {
        if (onSuccess != nullptr)
            onSuccess();
        return;
    }
    float changePerSecond = (target - current) / duration;
    ecs::add<cp::Cmd>(eid, gid).setAnimate(tag,
            [duration, &current, target, changePerSecond](unsigned eid, double dt) mutable {
        if (duration <= 0)
        {
            current = target;
            return State::success;
        }
        current += changePerSecond * dt;
        duration -= dt;
        return State::inProgress;
    }, onSuccess, onFailure);
}

void CmdFactory::animParamTo(const std::string &tag,
                             std::list<std::reference_wrapper<float>> current,
                             std::list<float> to, float duration)
{
    auto it1 = current.begin();
    auto it2 = to.begin();
    
    std::list<float> changePerSecond;
    while(it1 != current.end() && it2 != to.end())
    {
        changePerSecond.push_back((*(it2++) - *(it1++)) / duration);
    }

    ecs::add<cp::Cmd>(eid, gid).setAnimate(tag,
            [duration, current, to, changePerSecond](unsigned eid, double dt) mutable {
        if (duration <= 0)
        {
            auto it1 = current.begin();
            auto it2 = to.begin();
            while(it1 != current.end() && it2 != to.end())
            {
                auto& current = (*it1).get();
                current = *it2;
                it1++;
                it2++;
            }
            
            return State::success;
        }
        auto it1 = current.begin();
        auto it2 = changePerSecond.begin();
        while(it1 != current.end() && it2 != changePerSecond.end())
        {
            auto& current = (*it1).get();
            current += *it2 * dt;
            it1++;
            it2++;
        }
        duration -= dt;
        return State::inProgress;
    }, onSuccess, onFailure);
}

void CmdFactory::delay(double timeInterval)
{
    ecs::add<cp::Cmd>(eid, gid).setTick(std::to_string(tagUID++),
            [timeInterval](unsigned eid, double dt) mutable {
        if (timeInterval <= 0)
            return State::success;
        timeInterval -= dt;
        return State::inProgress;
    }, onSuccess, onFailure);
}

void CmdFactory::waituntil(double timeout, const std::function<bool()>& predicate)
{
    ecs::add<cp::Cmd>(eid, gid).setTick(std::to_string(tagUID++),
            [timeout, predicate](unsigned eid, double dt) mutable {
        if (timeout <= 0)
            return State::failure;
        timeout -= dt;
        if (predicate())
            return State::success;
        return State::inProgress;
    }, onSuccess, onFailure);
}