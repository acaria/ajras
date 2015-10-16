#include "CollectibleModel.h"

CollectibleModel::CollectibleModel()
{
    this->load();
}

CollectibleData CollectibleModel::get(std::string key)
{
    assert(this->models.find(key) != this->models.end());
    return this->models[key];
}

void CollectibleModel::addModel(CollectibleData data)
{
    this->models[data.key] = data;
}

void CollectibleModel::load()
{
    this->addModel({
        .key = "gold1",
        .currencyValue = 1,
        .stackability = 50,
        .spriteFrameName = "gold_1.png",
        .spriteRotate = false
    });
    this->currencyGroup.insert("gold1");
    
    this->addModel({
        .key = "gold2",
        .currencyValue = 2,
        .stackability = 50,
        .spriteFrameName = "gold_2.png",
        .spriteRotate = false
    });
    this->currencyGroup.insert("gold2");
    
    this->addModel({
        .key = "gold4",
        .currencyValue = 4,
        .stackability = 50,
        .spriteFrameName = "gold_3.png",
        .spriteRotate = false
    });
    this->currencyGroup.insert("gold3");
    
    this->addModel({
        .key = "gold8",
        .currencyValue = 8,
         .stackability = 50,
        .spriteFrameName = "gold_4.png",
        .spriteRotate = false
    });
    this->currencyGroup.insert("gold4");
    
    this->addModel({
        .key = "gold16",
        .currencyValue = 16,
        .stackability = 50,
        .spriteFrameName = "gold_5.png",
        .spriteRotate = false
    });
    this->currencyGroup.insert("gold5");
    
    this->addModel({
        .key = "treasure_cup",
        .currencyValue = 100,
        .stackability = 1,
        .spriteFrameName = "gold_6.png",
        .spriteRotate = true
    });
}