#pragma once

class IMapData;

#include "BaseSystem.h"
#include "ECSGroup.h"

class UpdaterSystem : public BaseSystem
{
public:
    UpdaterSystem(lib::EcsGroup& ecs) : BaseSystem(ecs) {}
    
    ~UpdaterSystem() {}
    
    void init(IMapData* data);
    
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
private:
    IMapData* data;
};