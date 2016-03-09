#include "CollectibleModel.h"
#include "CoreLib.h"

CollectibleData* CollectibleModel::get(const std::string& key)
{
    if (!this->initialized)
    {
        this->loadAll();
    }
    assert(this->models.find(key) != this->models.end());
    return &this->models[key];
}

void CollectibleModel::addModel(CollectibleData data)
{
    this->models[data.key] = data;
}

std::list<CollectibleData*> CollectibleModel::genReward(lib::Random& rEngine, const std::string& key)
{
    if (!this->initialized)
    {
        this->loadAll();
    }
    auto result = std::list<CollectibleData*>();
    assert(this->rewards.find(key) != this->rewards.end());
    
    for(auto reward : this->rewards[key])
    {
        if (reward.size() == 1)
            result.push_back(this->get(reward.front()));
        else
            result.push_back(this->get(rEngine.select(reward)));
    }
    
    return result;
}

void CollectibleModel::loadAll()
{
    this->initialized = true;
    
    auto path = cc::FileUtils::getInstance()->fullPathForFilename("templates/collectible.plist");
    auto rawData = cc::FileUtils::getInstance()->getValueMapFromFile(path);
    
    for(auto row : rawData)
    {
        auto info = row.second.asValueMap();
        
        bool isCurrency = (info.find("isCurrency") != info.end()) ?
                info.at("isCurrency").asBool() : false;
    
        unsigned stackability = (info.find("stackability") != info.end()) ?
                info.at("stackability").asInt() : 0;
        
        bool spriteRotate = (info.find("spriteRotate") != info.end()) ?
                info.at("spriteRotate").asBool() : false;
        
        this->addModel({
            .key = row.first,
            .currencyValue = (unsigned)info.at("currencyValue").asInt(),
            .stackability = stackability,
            .spriteFrameName = info.at("spriteName").asString(),
            .spriteRotate = spriteRotate,
            .currency = isCurrency
        });
        
        if (isCurrency)
            this->currencyGroup.insert(row.first);
    }
    
    path = cc::FileUtils::getInstance()->fullPathForFilename("templates/reward.plist");
    auto rawData2 = cc::FileUtils::getInstance()->getValueMapFromFile(path);
    
    for(auto row : rawData2)
    {
        std::list<std::list<std::string>> collectibleList;
        for(auto itemStr : row.second.asValueVector())
        {
            std::list<std::string> items;
            lib::split(itemStr.asString(), items, ",", true);
            collectibleList.push_back(items);
        }
        this->rewards[row.first] = collectibleList;
    }
}

