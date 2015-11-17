#pragma once
#include "BaseTickSystem.h"
#include "ECSGroup.h"

class MoveSystem : public BaseTickSystem
{
public:
    MoveSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    virtual ~MoveSystem() {}
    
    virtual void tick(double dt) final;
    virtual void animate(double dt, double tp) final {}
};