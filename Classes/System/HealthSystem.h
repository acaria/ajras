#pragma once

#include "ECSGroup.h"
#include "BaseTickSystem.h"
#include "Event.h"

class HealthSystem : public BaseTickSystem
{
public:
    HealthSystem(lib::EcsGroup& ecs);
    virtual ~HealthSystem() {}
    
    //overrides
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {};
    
    lib::Subject<void(unsigned eid, int health)> onHealthChanged;
};