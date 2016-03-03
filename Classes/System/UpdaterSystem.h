#pragma once

class IMapData;

#include "BaseSystem.h"
#include "ECSGroup.h"

class UpdaterSystem : public BaseSystem
{
public:
    ~UpdaterSystem() {}
    
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {};
};