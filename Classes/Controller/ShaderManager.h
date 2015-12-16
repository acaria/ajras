#pragma once

class ShaderManager
{
public:
    void load();
    
    void setLightConfig(const cc::Color3B& lightColor,
                        const cc::Color3B& ambiantColor,
                        float brightness,
                        float cutOffRadius,
                        float halfRadius);
    
    PROPERTY(cc::Vec3, lightPos, LightPos);
};