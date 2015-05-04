#pragma once
#include "BaseTickSystem.h"
#include "Event.h"

class PositionComponent;
class GateInfo;
class GateMap;
class CollisionComponent;

class TransitSystem : public BaseTickSystem
{
public:
    TransitSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    virtual ~TransitSystem() {}
    
    virtual void tick(double dt) final;
    virtual void animate(double dt, double tp) final {}
    
    lib::Subject<void(unsigned eid, GateMap gate)> onGateTriggered;
    
private:
    

    void gateringEnter(unsigned targetEntity,
                       const cocos2d::Vec2& targetPoint,
                       const GateMap& gateMap);
    
    std::pair<float, cc::Vec2> processing(GateInfo info,
                                          PositionComponent& pos,
                                          CollisionComponent& col);
};