#pragma once

struct LightConfig
{
    enum SpotType
    {
        player,
        object
    };
    
    struct SpotInfo
    {
        cc::Color3B     color;
        cc::Vec3        pos;
        float           brightness;
        float           cutOffRadius;
        float           halfRadius;
    };
    
    using SpotDefaultMapConfig = std::map<SpotType, SpotInfo>;
    
    void initAmbiant(const cc::Color3B& ambiantColor);
    void initSpot(SpotType type, const SpotInfo& info);
    
    cc::Color3B                     ambiantColor = {255,255,255};
    SpotDefaultMapConfig            defaultCfg;
    std::vector<SpotInfo>           spots;
};

