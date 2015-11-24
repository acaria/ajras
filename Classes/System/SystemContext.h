#pragma once

#include "IMapData.h"
#include "LayeredContainer.h"
#include "ECSGroup.h"

struct SystemContext
{
    IMapData*           data = nullptr;
    LayeredContainer*   view = nullptr;
    lib::EcsGroup*      ecs = nullptr;
};