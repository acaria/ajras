#pragma once
#include "BaseTickSystem.h"
#include "ECSGroup.h"

class IMapData;

class DebugSystem : public BaseTickSystem
{
public:
    DebugSystem(lib::EcsGroup& ecs);
    virtual ~DebugSystem() {}
    
    void init(cc::Node *view, IMapData *data);
    
    //overrides
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}


private:

    IMapData* data = nullptr;
    cc::Node* debugLayer = nullptr;
};