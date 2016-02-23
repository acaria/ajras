#pragma once

struct RegroupTeamInfo
{
    bool processed = false;
    std::list<unsigned> leftIds;
    std::list<unsigned> readyIds;
};

