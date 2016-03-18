#include "ProfileStatsInfo.h"

ProfileStatsInfo::ProfileStatsInfo(const cc::ValueMap& map)
{
    if (map.find("physics") != map.end())
    {
        auto &mData = map.at("physics").asValueMap();
        
        //extract bounds
        std::vector<std::string> split;
        lib::split(mData.at("rect").asString(), split, ", ");
        
        this->physics = {
            .speed = (mData.find("speed") != mData.end()) ?
                mData.at("speed").asFloat() : 0.0f,
            .strength = (mData.find("strength") != mData.end()) ?
                mData.at("strength").asFloat() : 0.0f,
            .acceleration = mData.find("acceleration") != mData.end() ?
                mData.at("acceleration").asFloat() : 0.0f,
            .deceleration = mData.find("deceleration") != mData.end() ?
                mData.at("deceleration").asFloat() : 0.0f,
            .bounds = cc::Rect(std::stoi(split[0]), std::stoi(split[1]),
                               std::stoi(split[2]), std::stoi(split[3])),
            .category = mData.at("cat").asString(),
            .weight = mData.find("weight") != mData.end() ?
                mData.at("weight").asFloat() : 0.0f
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

        lib::Nullable<std::string> animKey = nullptr;
        if (mData.find("anim_key") != mData.end())
            animKey = mData.at("anim_key").asString();

        this->melee = {
            .type = mData.at("type").asString(),
            .range = {std::stof(rangeSplit[0]), std::stof(rangeSplit[1])},
            .stamina = mData.at("stamina").asFloat(),
            .recoil = { .speed = std::stof(recoilSplit[0]), .duration = std::stof(recoilSplit[1])},
            .animKey = animKey,
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
    
    if (map.find("orientation") != map.end())
    {
        this->orientation = map.at("orientation").asBool();
    }
}