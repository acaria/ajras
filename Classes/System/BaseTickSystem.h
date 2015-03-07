#pragma once
#include "Headers.h"

class BaseTickSystem
{
public:
    BaseTickSystem(lib::EcsGroup& ecs) : ecs(ecs) {}
    virtual ~BaseTickSystem() {}
    
    virtual void tick(double dt) = 0;
    virtual void animate(double dt, double tp) = 0;
    
protected:
    lib::EcsGroup& ecs;
};