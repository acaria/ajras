#pragma once

#include "components.h"
#include "ecsGroup.h"
#include "Defines.h"

class CmdFactory
{
public:
    static void goTo(lib::EcsGroup* ecs, unsigned eid, cc::Vec2 target, float nearDistance);
    static void goTo(lib::EcsGroup* ecs, unsigned eid, std::list<cc::Vec2> waypoints, float nearDistance);
    static void delay(lib::EcsGroup* ecs, unsigned eid,
                      double timeInterval, const std::function<void()>& success);
    static void lightCfg(lib::EcsGroup* ecs,
                         float duration,
                         const def::shader::LightParam& param,
                         float value);
    static void lightCfg(lib::EcsGroup* ecs,
                         float duration,
                         const def::shader::LightParam& param,
                         const cc::Color3B& value);
    static void lightPos(lib::EcsGroup* ecs, float duration, const cc::Vec3& dest);
    static void lightPos(lib::EcsGroup* ecs, unsigned eid, float duration,
                         const cc::Vec2& margin);
    static void lightFollow(lib::EcsGroup* ecs, unsigned eid, const cc::Vec2& margin);
};