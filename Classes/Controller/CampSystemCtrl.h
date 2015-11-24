#pragma once

class CampData;
class PlayerData;
class WarpMap;

#include "Defines.h"

#include "SystemFacade.h"
#include "SystemContext.h"

#include "Randgine.h"
#include "LayeredContainer.h"
#include "Event.h"
#include "GameCamera.h"

class CampSystemCtrl
{
public:
    CampSystemCtrl();
    ~CampSystemCtrl();
    
    void load(GameCamera* cam, cc::Node* view, PlayerData *player, CampData* data);
    void start();
    void clear();
    
    void tick(double dt);
    void animate(double dt, double tickPercent);
    
    SystemDispatcher& getDispatcher();
    
private:
    const unsigned GROUP_INDEX = 1;
    
    //system
    SystemFacade        systemFacade;
    SystemContext       context;
    SystemDispatcher    dispatcher;
    
    void loadSystems(LayeredContainer* view, IMapData* data);
    
    //local ecs
    lib::EcsGroup   ecsGroup;
    
    //data
    CampData* data;
    
    //view
    LayeredContainer* mapView = nullptr;
    GameCamera* cam = nullptr;
    
    //event
    std::vector<lib::Registration> eventRegs;
    
    lib::Random& random;
    
    PlayerData* playerData = nullptr;
};