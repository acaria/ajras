#pragma once
#include "BaseSystem.h"
#include "ECSGroup.h"

class IMapData;

class MoveSystem : public BaseSystem
{
public:
    virtual ~MoveSystem() {}
    
    void init() override;
    
    virtual void tick(double dt) final;
    virtual void animate(double dt, double tp) final {}
};