#pragma once
#include "Headers.h"

class RoomSystemCtrl
{
public:
    RoomSystemCtrl(): renderSystem(ecsGroup),
                      collisionSystem(ecsGroup),
                      moveSystem(ecsGroup),
                      gateSystem(ecsGroup),
                      inputSystem(ecsGroup),
                      controlSystem(ecsGroup),
                      meleeSystem(ecsGroup),
                      targetSystem(ecsGroup),
                      aiSystem(ecsGroup)
    {
    }

    void load(GameScene* view, MapData* data);
    RoomData* changeRoom(unsigned roomIndex, unsigned gateIndex, const std::vector<unsigned>& eids);

    void tick(double dt);
    void animate(double dt, double tickPercent);

private:

    //systems
    RenderSystem    renderSystem;
    CollisionSystem collisionSystem;
    MoveSystem      moveSystem;
    GateSystem      gateSystem;
    InputSystem     inputSystem;
    ControlSystem   controlSystem;
    MeleeSystem     meleeSystem;
    TargetSystem    targetSystem;
    AISystem        aiSystem;
    
    //local ecs
    lib::EcsGroup   ecsGroup;
    
    //data
    MapData*    data;
    std::map<unsigned, RoomLayer*> roomViews;
    
    //view
    GameScene* gView = nullptr;
};