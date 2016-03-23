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

/*void CmdFactory::lightCfg(float duration, const LightConfig::LightParam& param,
                          const LightValue& value)
{
    
}

void CmdFactory::lightSpotCfg(float duration, unsigned spotIndex,
                          const LightConfig::LightParam& param, const LightValue& value)
{
    auto lightCfg = GameCtrl::instance()->getEffects().getLightConfig();
    auto spot = lightCfg.spots[spotIndex];
    
    LightValue current;
    std::string tag;
    
    switch(param)
    {
        case LightConfig::LightParam::brightness:
            current = LightValue();
            current.set<float>(spot.brightness);
            tag = "brightness";
            break;
        case LightConfig::LightParam::cutOffRadius:
            current = spot.cutOffRadius;
            tag = "cutoffradius";
            break;
        case LightConfig::LightParam::halfRadius:
            current = spot.halfRadius;
            tag = "halfradius";
            break;
        default:
            Log("invalid light Cfg CMD");
            return;
    }
    
    if (duration == 0)
        return;
    
    float inc = 1 / (def::ticksPerSecond * duration);
    float amount = 0;
    ecs::add<cp::Cmd>(eid, gid).setTick(tag,
            [param, current, value, amount, inc](unsigned eid, double dt) mutable {
        if (amount >= 1.0)
            return State::success;
        
        amount += inc;
        GameCtrl::instance()->getEffects().setLightParam(param,
            current + (value - current) * amount);
        
        return State::inProgress;
    }, onSuccess, onFailure);
}

void CmdFactory::lightCfg(float duration,
                          const LightConfig::LightParam& param, const cc::Color3B& value)
{
    auto eid = GameCtrl::instance()->getData().getPlayerData()->getEntityFocusID();
    auto lightCfg = GameCtrl::instance()->getEffects().getLightConfig();
    
    cc::Color3B current;
    std::string tag;

    switch(param)
    {
        case LightConfig::LightParam::ambiantColor:
            current = lightCfg.ambiantColor;
            tag = "ambiantcolor";
            break;
        default:
            Log("invalid light Cfg CMD");
            return;
    }
    
    float inc = 1 / (def::ticksPerSecond * duration);
    float amount = 0;
    ecs::add<cp::Cmd>(eid, gid).setTick(tag,
            [current, value, inc, amount, param](unsigned eid, double dt) mutable {
        if (amount >= 1.0)
            return State::success;
        
        amount += inc;
        GameCtrl::instance()->getEffects().setLightParam(param,
            ColorUtil::interpolate(current, value, amount));
        
        return State::inProgress;
    }, onSuccess, onFailure);
}

void CmdFactory::lightPos(float duration, const cc::Vec2& dest)
{
    auto eid = GameCtrl::instance()->getData().getPlayerData()->getEntityFocusID();
    
    cc::Vec2 from = GameCtrl::instance()->getEffects().getLightPos();
    
    float inc = 1 / (def::ticksPerSecond * duration);
    float amount = 0;
    ecs->add<cp::Cmd>(eid).setTick("lightposition",
            [from, dest, inc, amount](unsigned eid, double dt) mutable {
        if (amount >= 1.0)
            return State::success;
                                   
        amount += inc;
        GameCtrl::instance()->getEffects().setLightPos(cc::Vec2(
            from.x * (1 - amount) + dest.x * amount,
            from.y * (1 - amount) + dest.y * amount
        ));
                                   
        return State::inProgress;
    });
}

void CmdFactory::lightPos(float duration, const cc::Vec2& margin)
{
    if (!ecs::has<cp::Render>(eid))
        return;
    auto sprite = ecs::get<cp::Render>(eid).sprite;
    
    float inc = 1 / (def::ticksPerSecond * duration);
    float amount = 0;
    ecs::add<cp::Cmd>(eid, gid).setTick("lightposition",
            [sprite, inc, amount, margin, this](unsigned eid, double dt) mutable {
        if (amount >= 1.0)
            return State::success;
                                   
        amount += inc;
        auto p = sprite->convertToWorldSpace(margin);
        GameCtrl::instance()->getEffects().setLightPos({p.x, p.y});
        return State::inProgress;
    }, onSuccess, onFailure);
}

void CmdFactory::lightFollow(const cc::Vec2& margin)
{
    if (!ecs::has<cp::Render>(eid))
        return;
    auto sprite = ecs::get<cp::Render>(eid).sprite;
    
    ecs::add<cp::Cmd>(eid, gid).setTick("lightfollow",
            [sprite, margin](unsigned eid, double dt) mutable {
        auto p = sprite->convertToWorldSpace(margin);
        GameCtrl::instance()->getEffects().setLightPos({p.x, p.y});
        return State::inProgress;
    }, onSuccess, onFailure);
}*/

void CmdFactory::animParamBy(const std::string &tag, float &current, float change, float duration)
{
    return animParamTo(tag, current, current + change, duration);
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