#pragma once
#include "Headers.h"

class RoomSystemCtrl
{
public:
    RoomSystemCtrl(): renderSystem(ecsGroup),
                      collisionSystem(ecsGroup),
                      moveSystem(ecsGroup),
                      transSystem(ecsGroup),
                      inputSystem(ecsGroup),
                      meleeSystem(ecsGroup),
                      targetSystem(ecsGroup),
                      aiSystem(ecsGroup)
    {
    }

    void loadRoom(RoomLayer* view, RoomData* data);
    
    RoomData* changeRoom(unsigned roomIndex, unsigned gateIndex, const std::vector<unsigned>& eids);

    void tick(double dt);
    void animate(double dt, double tickPercent);

private:

    void loadStart(RoomLayer* view, RoomData* data);
    void loadCommon(RoomLayer* view, RoomData* data);
    void loadEnd(RoomLayer* view, RoomData* data);

    //systems
    RenderSystem    renderSystem;
    CollisionSystem collisionSystem;
    MoveSystem      moveSystem;
    TransitSystem   transSystem;
    InputSystem     inputSystem;
    MeleeSystem     meleeSystem;
    TargetSystem    targetSystem;
    AISystem        aiSystem;
    
    //local ecs
    lib::EcsGroup   ecsGroup;
};