#pragma once
#include "Defines.h"
#include "RenderSystem.h"
#include "CollisionSystem.h"
#include "TransitSystem.h"
#include "MeleeSystem.h"
#include "TargetSystem.h"
#include "AISystem.h"
#include "TargetSystem.h"
#include "InputSystem.h"
#include "MoveSystem.h"
#include "InteractSystem.h"

#if ECSYSTEM_DEBUG
#include "DebugSystem.h"
#endif

#include "Event.h"

class RoomData;
class LayeredNode;
class MissionScene;
class GateMap;

class RoomSystemCtrl
{
public:
    RoomSystemCtrl();
    
    void loadRoom(LayeredNode* view, RoomData* data);
    
    RoomData* changeRoom(unsigned roomIndex, unsigned gateIndex, const std::vector<unsigned>& eids);

    void tick(double dt);
    void animate(double dt, double tickPercent);

    //dispays
    void hideObjects(float duration);
    void showObjects(float duration);
    
    //events
    lib::Subject<void(unsigned roomID, unsigned eid, int health)> onHealthChanged;
    lib::Subject<void(unsigned roomID, unsigned eid, GateMap gate)> onGateTriggered;

private:

    void registerEvents();
    std::vector<lib::Registration> eventRegs;

    //systems
    RenderSystem    renderSystem;
    CollisionSystem collisionSystem;
    MoveSystem      moveSystem;
    TransitSystem   transSystem;
    InputSystem     inputSystem;
    MeleeSystem     meleeSystem;
    TargetSystem    targetSystem;
    AISystem        aiSystem;
    InteractSystem  interactSystem;
#if ECSYSTEM_DEBUG
    DebugSystem     debugSystem;
#endif
    
    //local ecs
    lib::EcsGroup   ecsGroup;
};