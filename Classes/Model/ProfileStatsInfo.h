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
    
    struct Recoil
    {
        float duration;
        float speed;
    };
    
    struct Melee
    {
        std::string                type;
        cc::Size                   range;
        float                      stamina;
        Recoil                     recoil;
        lib::Nullable<std::string> animKey;
        float                      triggerRatio;
    };
    
    lib::Nullable<Move>     move;
    lib::Nullable<Sight>    sight;
    lib::Nullable<Melee>    melee;
    lib::Nullable<float>    health;
    lib::Nullable<float>    stamina;
};