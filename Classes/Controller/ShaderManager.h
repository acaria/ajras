#pragma once

#include "Defines.h"

class ShaderManager
{
public:

    void load();
    
    void setLightConfig(const def::shader::LightConfig& config);
    void setLightConfig(const cc::Color3B& lightColor,
                        const cc::Color3B& ambiantColor,
                        float brightness,
                        float cutOffRadius,
                        float halfRadius);
    
    const def::shader::LightConfig& getLightConfig();
    
    void setLightParam(const def::shader::LightParam& param, float value);
    void setLightParam(const def::shader::LightParam& param, const cc::Color3B& value);
    
    PROPERTY(cc::Rect, frame,    Frame);
    PROPERTY(cc::Vec2, lightPos, LightPos);
    
private:
    def::shader::LightConfig lightConfig;
};