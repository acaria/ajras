#pragma once

struct LightConfig
{
    struct SpotInfo
    {
        cc::Color3B     color;
        cc::Vec3        pos;
        float           brightness;
        float           cutOffRadius;
        float           halfRadius;
    };
    
    struct ObjectInfo
    {
        std::pair<cc::Color3B, cc::Color3B> color;
        std::pair<cc::Size, cc::Size>       size;
        std::pair<unsigned, unsigned>       opacity;
        cc::Vec2 pos;
    };
    
    cc::Color3B ambiantColor = {255,255,255};
    SpotInfo spot = {
        .color = {255,255,255}, .pos = {0,0,0},
        .brightness = 0, .cutOffRadius = 0, .halfRadius = 0
    };
    std::map<std::string, ObjectInfo>   objects;
};

