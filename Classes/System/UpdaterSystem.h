#pragma once
#include "BaseSystem.h"
#include "ECSGroup.h"

class UpdaterSystem : public BaseSystem
{
public:
    UpdaterSystem(lib::EcsGroup& ecs) : BaseSystem(ecs) {}
    
    ~UpdaterSystem() {}
    
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
};