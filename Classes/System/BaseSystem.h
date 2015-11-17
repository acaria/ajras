#pragma once
#include "ECSGroup.h"

class BaseSystem
{
public:
    BaseSystem(lib::EcsGroup& ecs) : ecs(ecs) {}
    virtual ~BaseSystem() {}
    
    virtual void tick(double dt) = 0;
    virtual void animate(double dt, double tp) = 0;
    
protected:
    lib::EcsGroup& ecs;
};