#pragma once

class RoomData;
class IMapData;

#include "BaseSystem.h"
#include "ECSGroup.h"
#include "CoreLib.h"
#include "SweptAABB.h"
#include "Components.h"
#include "CollisionInfo.h"
#include "Event.h"

class CollisionSystem : public BaseSystem
{
public:
    CollisionSystem(lib::EcsGroup& ecs) : BaseSystem(ecs) {}
    virtual ~CollisionSystem();
    
    void init(IMapData* data);
    
    virtual void tick(double dt) final;
    virtual void animate(double dt, double tp) final {}
    
    lib::Subject<void(unsigned, const cp::GearComponent&)> onGearChanged;
    
private:
    lib::Box bounce(const PositionComponent &cpPos,
                    const PhysicsComponent &cpCol,
                    const cocos2d::Rect& target);
    
    cc::Vec2 slide(const cc::Rect &src,
                   const cc::Rect &target);
    
    //properties
    CollisionInfo* collisionData;
};