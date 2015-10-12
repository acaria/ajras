#pragma once

#include "ProfileModel.h"
#include "BehaviourModel.h"
#include "CollectibleModel.h"
#include "Singleton.h"

class ModelProvider : public Singleton<ModelProvider>
{
public:
    ProfileModel        profile;
    BehaviourModel      behaviour;
    CollectibleModel    collectible;
};

