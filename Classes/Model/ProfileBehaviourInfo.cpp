#include "ProfileBehaviourInfo.h"
#include "CoreLib.h"

ProfileBehaviourInfo::ProfileBehaviourInfo(const cc::ValueMap& map)
{
    assert(map.find("key") != map.end());
    this->key = map.at("key").asString();
    
    if (map.find("sight") != map.end())
    {
        std::vector<std::string> sightSplit;
        lib::split(map.at("sight").asString(), sightSplit, ", ", true);
        assert(sightSplit.size() == 2);
        sightRange = { MIN(std::stof(sightSplit[0]), std::stof(sightSplit[1])),
                       MAX(std::stof(sightSplit[0]), std::stof(sightSplit[1])) };
    }
    
    if (map.find("mood") != map.end())
        moodCategory = map.at("mood").asString();
    if (map.find("sleep") != map.end())
        sleepCategory = map.at("sleep").asString();
}
