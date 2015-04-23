#pragma once
#include "BaseTickSystem.h"
#include "Event.h"

class PositionComponent;
class GateInfo;
class GateComponent;
class WarpComponent;
class CollisionComponent;

class TransitSystem : public BaseTickSystem
{
public:
    TransitSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {}
    virtual ~TransitSystem() {}
    
    virtual void tick(double dt) final;
    virtual void animate(double dt, double tp) final {}
    
    event::Subject<void(unsigned eid, int health)> onRoomChanged;
    
private:
    

    void gateringEnter(unsigned targetEntity,
                       const cocos2d::Vec2& targetPoint,
                       const GateComponent& srcGate);
    
    void gateringLeave(unsigned eid,
                       unsigned roomIdx,
                       unsigned gateIdx);
    
    void warpingEnter(unsigned targetEntity,
                      const cocos2d::Vec2& targetPoint,
                      const WarpComponent& srcWarp);
    
    std::pair<float, cc::Vec2> processing(GateInfo info,
                                          PositionComponent& pos,
                                          CollisionComponent& col);
};