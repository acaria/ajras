#pragma once

#include "components.h"
#include "ecsGroup.h"
#include "Defines.h"

class CmdFactory
{
public:
    CmdFactory(lib::EcsGroup* ecs, unsigned eid):ecs(ecs), eid(eid) {};
    static CmdFactory at(lib::EcsGroup* ecs, unsigned eid);

    void goTo(cc::Vec2 target, float nearDistance);
    void goTo(std::list<cc::Vec2> waypoints, float nearDistance);
    void delay(double timeInterval, const std::function<void()>& success);
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
    lib::EcsGroup* ecs;
    unsigned eid;
};