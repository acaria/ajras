#include "BehaviourModel.h"
#include "BehaviourData.h"
#include "CoreLib.h"

BehaviourData* BehaviourModel::get(const std::string &bName)
{
    if (!lib::hasKey(this->behaviours, bName))
    {
        auto path = cc::FileUtils::getInstance()->fullPathForFilename("behaviours/" + bName + ".txt");
        this->behaviours[bName] = new BehaviourData(path);
    }
    
    return this->behaviours[bName];
}

void BehaviourModel::clear()
{
    for(auto i : this->behaviours)
    {
        delete i.second;
    }
    this->behaviours.clear();
}

BehaviourModel::~BehaviourModel()
{
    this->clear();
}