#pragma once

#include "Defines.h"
#include "GateMap.h"

struct TransitInfo
{
    bool processed = false;
    std::list<unsigned>         teamLeftIds;
    std::list<unsigned>         teamReadyIds;
    std::map<unsigned, GateMap> processing;
    
    cc::Vec2 deployTeamUnit(const GateMap& gate,
                            def::team::Formation formation, unsigned position);
};

