#pragma once

#include "CollectibleData.h"
#include "Random.h"

class CollectibleModel
{
public:
    CollectibleData* get(const std::string& key);
    std::list<CollectibleData*> genReward(lib::Random& rEngine, const std::string& key);
    void loadAll();
    
private:
    bool initialized = false;
    void addModel(CollectibleData data);
    
    std::map<std::string, CollectibleData>         models;
    std::map<std::string,
             std::list<std::list<std::string>>>    rewards;
    std::set<std::string> currencyGroup;
};

