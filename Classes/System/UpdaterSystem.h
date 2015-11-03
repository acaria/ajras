#pragma once
#include "BaseTickSystem.h"
#include "ECSGroup.h"

class UpdaterSystem : public BaseTickSystem
{
public:
    UpdaterSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    
    ~UpdaterSystem() {}
    
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
};