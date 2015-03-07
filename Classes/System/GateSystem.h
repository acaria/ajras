#pragma once
#include "Headers.h"

class GateSystem : public BaseTickSystem
{
public:
    GateSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    virtual ~GateSystem() {}
    
    virtual void tick(double dt) final;
    virtual void animate(double dt, double tp) final {}
    
private:
    void gateringEnter(unsigned targetEntity,
                  const cocos2d::Vec2& targetPoint,
                  const GateComponent& srcGate);
    
    void gateringLeave(unsigned eid,
                         unsigned roomIdx,
                         unsigned gateIdx);
};