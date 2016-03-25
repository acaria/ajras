#pragma once

#include "LightConfig.h"

struct LightManager
{
    //attributes
    LightConfig defaultConfig;
    LightConfig currentConfig;
    
    void init(const LightConfig& config)
    {
        currentConfig = config;
        defaultConfig = config;
        currentConfig.spot.brightness = 0;
        currentConfig.spot.cutOffRadius = 0;
    }
};

