#pragma once

class CampData;
class PlayerData;
class WarpMap;

#include "ControlSystem.h"
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

#include "Randgine.h"
#include "LayeredNode.h"
#include "Event.h"
#include "GameCamera.h"

class CampSystemCtrl
{
public:
    CampSystemCtrl();
    ~CampSystemCtrl();
    
    void load(GameCamera* cam, cc::Node* view,
              PlayerData *player, CampData* data);
    void start();
    void clear();
    
    void tick(double dt);
    void animate(double dt, double tickPercent);
    
    ControlSystem* getCtrlSystem();
    
    lib::Subject<void(unsigned, int)>     onHealthChanged;
    lib::Subject<void(unsigned, WarpMap)> onWarpTriggered;
    
private:
    //systems
    ControlSystem controlSystem;
    RenderSystem    renderSystem;
    CollisionSystem collisionSystem;
    MoveSystem      moveSystem;
    TransitSystem   transSystem;
    InputSystem     inputSystem;
    MeleeSystem     meleeSystem;
    TargetSystem    targetSystem;
    AISystem        aiSystem;
    InteractSystem  interactSystem;
    
    //local ecs
    lib::EcsGroup   ecsGroup;
    
    //data
    CampData* data;
    
    //view
    cc::Node* view = nullptr;
    LayeredNode* mapView = nullptr;
    GameCamera* cam = nullptr;
    
    //event
    std::vector<lib::Registration> eventRegs;
    
    lib::Random& random;
    
    PlayerData* playerData = nullptr;
};