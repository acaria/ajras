#pragma once
#include "LightConfig.h"

struct LightSpotComponent
{
    cc::Sprite*                         halo = nullptr;
    std::pair<cc::Size, cc::Size>       defaultSize;
    std::pair<cc::Color3B, cc::Color3B> defaultColor;
    std::pair<int, int>                 defaultOpacity;
};

