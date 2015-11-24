#pragma once

class RoomData;
class IMapData;

#include "BaseSystem.h"
#include "ECSGroup.h"
#include "CoreLib.h"
#include "SweptAABB.h"
#include "Components.h"
#include "CollisionInfo.h"

class CollisionSystem : public BaseSystem
{
public:
    CollisionSystem() {}
    virtual ~CollisionSystem();
    
    virtual void init() override;
    
    virtual void tick(double dt) final;
    virtual void animate(double dt, double tp) final {}
    
    void onDecorCollision(unsigned eid, cc::Vec2 diff);
    void onAgentCollision(unsigned eid, unsigned tid, cc::Vec2 diff);
    
    lib::Subject<void(unsigned, const cp::GearComponent&)> onGearChanged;
    
private:
    
    //properties
    CollisionInfo* collisionData;
    
    std::vector<lib::Registration> sysRegs;
};