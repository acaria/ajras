#pragma once
#include "BaseTickSystem.h"
#include "Components.h"
#include "ECSGroup.h"
#include "CollisionInfo.h"

class InteractSystem : public BaseTickSystem
{
public:
    InteractSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    
    void init(CollisionInfo* collision);
    void tick(double dt) final;
    void animate(double dt, double tickPercent) final {}
    
private:
    void triggerAction(unsigned eid, InteractComponent& interact);
    CollisionInfo* collision;
};