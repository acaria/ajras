#pragma once

#include "ProfileModel.h"
#include "BehaviourModel.h"
#include "CollectibleModel.h"

struct ModelFacade
{
    ProfileModel        profile;
    BehaviourModel      behaviour;
    CollectibleModel    collectible;
};

