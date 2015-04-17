#pragma once
#include "BaseTickSystem.h"
#include "ECSGroup.h"

class InputSystem : public BaseTickSystem
{
public:
    InputSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    
    ~InputSystem() {}
    
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
};