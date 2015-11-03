#pragma once

#include "CoreLib.h"

struct ProfileStatsInfo
{
    ProfileStatsInfo() {}
    
    ProfileStatsInfo(const cc::ValueMap& map);
    struct Move
    {
        double          speed;
        double          acceleration;
        double          deceleration;
        bool            orientation;
    };
    
    struct Sight
    {
        double range;
    };
    
    struct Melee
    {
        std::string     type;
        cc::Size        range;
        float           stamina;
        std::string     animKey;
    };
    
    lib::Nullable<Move>     move;
    lib::Nullable<Sight>    sight;
    lib::Nullable<Melee>    melee;
    lib::Nullable<float>    health;
    lib::Nullable<float>    stamina;
};