#pragma once
#include "BaseSystem.h"
#include "ECSGroup.h"

class CleanerSystem : public BaseSystem
{
public:
    ~CleanerSystem() {}
    void init() override;

    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
private:
    std::list<unsigned> toRemove;
    
};
