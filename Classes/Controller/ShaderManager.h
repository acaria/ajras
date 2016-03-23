#pragma once

#include "Defines.h"
#include "LightConfig.h"

class ShaderManager
{
public:

    void load();
    
    void initLightConfig(const LightConfig& config);
    
    LightConfig& getLightConfig() { return lightConfig; }
    
    PROPERTY(cc::Rect, frame, Frame);
    
private:
    
    LightConfig lightConfig;
};