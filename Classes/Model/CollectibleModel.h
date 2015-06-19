#pragma once

#include "CollectibleData.h"

class CollectibleModel
{
public:
    CollectibleData get(std::string key);
private:
    void load();
    void addModel(CollectibleData data);
    
    std::map<std::string, CollectibleData> models;
};

