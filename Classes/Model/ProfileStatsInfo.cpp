#include "ProfileStatsInfo.h"

ProfileStatsInfo::ProfileStatsInfo(const cc::ValueMap& map)
{
    if (map.find("move") != map.end())
    {
        auto &mData = map.at("move").asValueMap();
        this->move = {
            .speed = mData.at("speed").asDouble(),
            .acceleration = mData.at("acceleration").asDouble(),
            .deceleration = mData.at("deceleration").asDouble(),
            .orientation = mData.at("orientation").asBool()
        };
    }
    
    if (map.find("sight") != map.end())
    {
        auto &siData = map.at("sight").asValueMap();
        this->sight = {
            .range = siData.at("range").asDouble()
        };
    }
    
    if (map.find("melee") != map.end())
    {
        auto &mData = map.at("melee").asValueMap();
        std::vector<std::string> rangeSplit;
        lib::split(mData.at("range").asString(), rangeSplit, ", ", true);
        
        std::vector<std::string> recoilSplit;
        lib::split(mData.at("recoil").asString(), recoilSplit, ", ", true);

        float triggerRatio = 1.0;
        if (mData.find("trigger_ratio") != mData.end())
            triggerRatio = mData.at("trigger_ratio").asFloat();

        this->melee = {
            .type = mData.at("type").asString(),
            .range = {std::stof(rangeSplit[0]), std::stof(rangeSplit[1])},
            .stamina = mData.at("stamina").asFloat(),
            .recoil = { .speed = std::stof(recoilSplit[0]), .duration = std::stof(recoilSplit[1])},
            .animKey = mData.at("anim_key").asString(),
            .triggerRatio = triggerRatio
        };
    }
    
    if (map.find("health") != map.end())
    {
        this->health = map.at("health").asFloat();
    }
    
    if (map.find("stamina") != map.end())
    {
        this->stamina = map.at("stamina").asFloat();
    }
}