#pragma once

#include "BaseSystem.h"
#include "ECSGroup.h"

class CmdSystem : public BaseSystem
{
public:
    ~CmdSystem() {}
    
    void init() override;
    
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final;
};