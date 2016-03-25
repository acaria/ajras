#pragma once

#include "components.h"
#include "ecsGroup.h"
#include "Defines.h"
#include "LightConfig.h"

class CmdFactory
{
public:
    using State = CmdComponent::State;
    using LightValue = lib::variant<float, cc::Vec3>;
    
    CmdFactory(unsigned gid, unsigned eid,
               const std::function<void()>& onSuccess = nullptr,
               const std::function<void()>& onFailure = nullptr)
               :gid(gid), eid(eid), onSuccess(onSuccess), onFailure(onFailure) {};
    static CmdFactory at(lib::EcsGroup* ecs, unsigned eid,
                         const std::function<void()>& onSuccess = nullptr,
                         const std::function<void()>& onFailure = nullptr);
    static CmdFactory at(unsigned groupID, unsigned eid,
                         const std::function<void()>& onSuccess = nullptr,
                         const std::function<void()>& onFailure = nullptr);

    void goTo(cc::Vec2 target, float nearDistance);
    void goBy(cc::Vec2 dir);
    void goTo(std::list<cc::Vec2> waypoints, float nearDistance);
    void delay(double timeInterval);
    void waituntil(double timeout, const std::function<bool()>& predicate);
    
    void animParamTo(const std::string& tag, float &current, float to, float duration);
    void animParamTo(const std::string& tag, std::list<std::reference_wrapper<float>> current,
                     std::list<float> to, float duration);
    void animParamBy(const std::string& tag, float &current, float changeBy, float duration);
    void animParamBy(const std::string& tag, std::list<std::reference_wrapper<float>> current,
                     std::list<float> changeBy, float duration);
    
private:
    static long tagUID;
    unsigned gid;
    unsigned eid;
    std::function<void()> onSuccess;
    std::function<void()> onFailure;
};