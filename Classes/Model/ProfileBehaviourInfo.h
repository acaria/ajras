#pragma once
#include "CoreLib.h"

struct ProfileBehaviourInfo
{
    ProfileBehaviourInfo() {}
    
    ProfileBehaviourInfo(const cc::ValueMap& map);
    
    std::string             key;
    std::pair<float, float> sightRange {0, 0};
    std::string             moodCategory {"neutral"};
    std::string             sleepCategory {"none"};
};