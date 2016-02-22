#pragma once

#include "components.h"
#include "ecsGroup.h"
#include "Defines.h"

class CmdFactory
{
public:
    using State = CmdComponent::State;
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
    void goTo(std::list<cc::Vec2> waypoints, float nearDistance);
    void delay(double timeInterval);
    void lightCfg(float duration,
                  const def::shader::LightParam& param,
                  float value);
    void lightCfg(float duration,
                  const def::shader::LightParam& param,
                  const cc::Color3B& value);
    //void lightPos(float duration, const cc::Vec2& dest);
    void lightPos(float duration, const cc::Vec2& margin);
    void lightFollow(const cc::Vec2& margin);
    
private:
    unsigned gid;
    unsigned eid;
    std::function<void()> onSuccess;
    std::function<void()> onFailure;
};