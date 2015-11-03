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
    
    struct Health
    {
        unsigned        value;
    };
    
    lib::Nullable<Move>     move;
    lib::Nullable<Sight>    sight;
    lib::Nullable<Melee>    melee;
    lib::Nullable<Health>   health;
};