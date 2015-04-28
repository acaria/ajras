#pragma once
#include "RenderSystem.h"
#include "CollisionSystem.h"
#include "TransitSystem.h"
#include "MeleeSystem.h"
#include "TargetSystem.h"
#include "AISystem.h"
#include "TargetSystem.h"
#include "InputSystem.h"
#include "MoveSystem.h"
#include "Event.h"

class RoomData;
class RoomLayer;
class GameScene;
class GateMap;

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
        this->registerEvents();
    }

    void loadRoom(RoomLayer* view, RoomData* data);
    
    RoomData* changeRoom(unsigned roomIndex, unsigned gateIndex, const std::vector<unsigned>& eids);

    void tick(double dt);
    void animate(double dt, double tickPercent);

    //dispays
    void hideObjects(float duration);
    void showObjects(float duration);
    
    //events
    event::Subject<void(unsigned roomID, unsigned eid, int health)> onHealthChanged;
    event::Subject<void(unsigned roomID, unsigned eid, GateMap gate)> onGateTriggered;

private:

    void registerEvents();
    std::vector<event::Registration> eventRegs;

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