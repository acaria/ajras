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
        .category = ColCat::currency,
        .currencyValue = 1,
        .stackable = 50,
        .spriteFrameName = "gold_1.png"
    });
    
    this->addModel({
        .key = "gold2",
        .category = ColCat::currency,
        .currencyValue = 2,
        .stackable = 50,
        .spriteFrameName = "gold_2.png"
    });
    
    this->addModel({
        .key = "gold4",
        .category = ColCat::currency,
        .currencyValue = 4,
        .stackable = 50,
        .spriteFrameName = "gold_3.png"
    });
    
    this->addModel({
        .key = "gold8",
        .category = ColCat::currency,
        .currencyValue = 8,
        .stackable = 50,
        .spriteFrameName = "gold_4.png"
    });
    
    this->addModel({
        .key = "gold16",
        .category = ColCat::currency,
        .currencyValue = 16,
        .stackable = 50,
        .spriteFrameName = "gold_5.png"
    });
    
    this->addModel({
        .key = "treasure_cup",
        .category = ColCat::object,
        .currencyValue = 100,
        .stackable = 1,
        .spriteFrameName = "gold_6.png"
    });
}