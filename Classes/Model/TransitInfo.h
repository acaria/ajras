#pragma once

struct TransitInfo
{
    bool processed = false;
    std::list<unsigned>         teamLeftIds;
    std::list<unsigned>         teamReadyIds;
    std::map<unsigned, GateMap> processing;
};

