#pragma once
#include "LightConfig.h"

struct LightSpotComponent
{
    int                     index;

    LightConfig::SpotInfo   current;
    LightConfig::SpotInfo   defaultRef;
};

