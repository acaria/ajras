#pragma once
#include "BaseSystem.h"
#include "ECSGroup.h"

class MoveSystem : public BaseSystem
{
public:
    MoveSystem(lib::EcsGroup& ecs) : BaseSystem(ecs) {}
    virtual ~MoveSystem() {}
    
    virtual void tick(double dt) final;
    virtual void animate(double dt, double tp) final {}
};