#pragma once

#include "Defines.h"

class ShaderManager
{
public:

    void load();
    
    void setLightConfig(const cc::Color3B& lightColor,
                        const cc::Color3B& ambiantColor,
                        float brightness,
                        float cutOffRadius,
                        float halfRadius);
    
    void resetLightConfig();
    
    const def::shader::LightConfig& getLightConfig();
    
    void setLightParam(const def::shader::LightParam& param, float value);
    void setLightParam(const def::shader::LightParam& param, const cc::Color3B& value);
    
    PROPERTY(cc::Rect, frame, Frame);
    PROPERTY(cc::Vec3, lightPos, LightPos);
    
private:
    def::shader::LightConfig lightConfig;
};