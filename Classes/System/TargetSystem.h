#pragma once
#include "BaseTickSystem.h"
#include "ECSGroup.h"

class TargetSystem : public BaseTickSystem
{
public:
    TargetSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    
    void init();
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
};