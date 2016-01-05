#pragma once

class FloorData;
class PlayerData;
class NodeRenderer;
class GateMap;

#include "SystemFacade.h"
#include "SystemContext.h"

#include "RoomSystemCtrl.h"
#include "LayeredContainer.h"
#include "Randgine.h"
#include "GameCamera.h"

class FloorSystemCtrl
{
public:
    FloorSystemCtrl();
    ~FloorSystemCtrl();
    
    void load(GameCamera* cam, cc::Node* view, PlayerData *player, FloorData* data);
    void start();
    void clear();
    
    //void displayDebug(cc::Node* view, FloorData* data);
    //cc::Sprite* displayMap(FloorData* data);
    
    void tick(double dt);
    void animate(double dt, double tickPercent);
    
    void changeEntityRoom(unsigned nextRoomIndex, unsigned eid, const GateMap& gate);
    void switchRoom(unsigned fromRoomIndex, unsigned toRoomIndex, unsigned eid, cc::Vec2 destPos);
    
    SystemDispatcher& getDispatcher();
    
private:
    //system
    SystemFacade        systemFacade;
    SystemContext       context;
    SystemDispatcher    dispatcher;
    void bindSystems();
    //local ecs
    lib::EcsGroup       ecsGroup;
    
    void showRoom(unsigned roomIndex,
                  std::function<void()> after);
    
    //data
    FloorData*                              data;
    std::map<unsigned, LayeredContainer*>   roomViews;
    std::map<unsigned, NodeRenderer*>       roomPreviews;
    std::map<unsigned, RoomSystemCtrl*>     roomSystems;
    
    //view
    cc::Node*   view = nullptr;
    GameCamera* cam = nullptr;
    
    //event
    std::vector<lib::Registration> eventRegs;
    
    PlayerData* playerData = nullptr;
};
