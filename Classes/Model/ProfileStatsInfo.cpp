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
        this->melee = {
            .type = mData.at("type").asString(),
            .range = mData.at("range").asDouble(),
            .animKey = mData.at("anim_key").asString()
        };
    }
    
    if (map.find("health") != map.end())
    {
        this->health = {
            .value = (unsigned)map.at("health").asInt()
        };
    }
}