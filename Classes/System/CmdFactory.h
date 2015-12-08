#pragma once

#include "components.h"
#include "ecsGroup.h"

class CmdFactory
{
public:
    static void goTo(lib::EcsGroup* ecs, unsigned eid, cc::Vec2 target, float nearDistance);
    static void goTo(lib::EcsGroup* ecs, unsigned eid, std::list<cc::Vec2> waypoints, float nearDistance);
    static void delay(lib::EcsGroup* ecs, unsigned eid,
                      double timeInterval, const std::function<void()>& success);
};