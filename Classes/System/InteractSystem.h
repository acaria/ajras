#pragma once
#include "BaseSystem.h"
#include "Components.h"
#include "ECSGroup.h"
#include "CollisionInfo.h"

class IMapData;

class InteractSystem : public BaseSystem
{
public:
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
private:
    void triggerAction(unsigned eid, InteractComponent& interact);
};