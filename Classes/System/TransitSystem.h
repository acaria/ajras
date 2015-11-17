#pragma once
#include "BaseSystem.h"
#include "Event.h"

class PositionComponent;
class GateInfo;
class GateMap;
class WarpMap;
class PhysicsComponent;

class TransitSystem : public BaseSystem
{
public:
    TransitSystem(lib::EcsGroup& ecs) : BaseSystem(ecs) {}
    virtual ~TransitSystem() {}
    
    virtual void tick(double dt) final;
    virtual void animate(double dt, double tp) final {}
    
    lib::Subject<void(unsigned eid, GateMap gate)> onGateTriggered;
    lib::Subject<void(unsigned eid, WarpMap warp)> onWarpTriggered;
    
private:
    

    void gateringEnter(unsigned targetEntity,
                       const cocos2d::Vec2& targetPoint,
                       const GateMap& gateMap);
    
    void warpingEnter(unsigned targetEntity,
                      const cocos2d::Vec2& targetPoint,
                      const WarpMap& warpMap);
    
    std::pair<float, cc::Vec2> processing(GateInfo info,
                                          PositionComponent& pos,
                                          PhysicsComponent& col);
};