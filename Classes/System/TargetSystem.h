#pragma once
#include "BaseSystem.h"
#include "ECSGroup.h"

class TargetSystem : public BaseSystem
{
public:
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
};