#pragma once

#include "components.h"
#include "ecsGroup.h"

class CmdFactory
{
public:
    static std::string goTo(lib::EcsGroup& ecs, unsigned eid, cc::Vec2 target, float nearDistance);
    static std::string goTo(lib::EcsGroup& ecs, unsigned eid, std::list<cc::Vec2> waypoints, float nearDistance);
};