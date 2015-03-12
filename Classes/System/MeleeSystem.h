#pragma once
#include "Headers.h"

class MeleeSystem : public BaseTickSystem
{
public:
    MeleeSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    
    virtual ~MeleeSystem() {}
    
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
};