#pragma once

#include "CoreLib.h"
#include "Defines.h"

struct ProfileStatsInfo
{
    ProfileStatsInfo() {}
    
    ProfileStatsInfo(const cc::ValueMap& map);
    struct Physics
    {
        float                       speed;
        float                       strength;
        float                       acceleration;
        float                       deceleration;
        cc::Rect                    bounds;
        std::string                 category;
        float                       weight;
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
    
    lib::Nullable<Physics>  physics;
    lib::Nullable<Melee>    melee;
    lib::Nullable<float>    health;
    lib::Nullable<float>    stamina;
    bool orientation = false;
};