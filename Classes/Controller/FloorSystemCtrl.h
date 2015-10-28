#pragma once

class FloorData;
class PlayerData;
class NodeRenderer;
class GateMap;

#include "ControlSystem.h"
#include "RoomSystemCtrl.h"
#include "LayeredContainer.h"
#include "Randgine.h"
#include "Event.h"
#include "GameCamera.h"

class FloorSystemCtrl
{
public:
    FloorSystemCtrl();
    ~FloorSystemCtrl();
    
    void load(GameCamera* cam, cc::Node* view,
              PlayerData *player, FloorData* data);
    void start();
    void clear();
    void displayDebug(cc::Node* view, FloorData* data);
    cc::Sprite* displayMap(FloorData* data);
    
    void tick(double dt);
    void animate(double dt, double tickPercent);
    
    void healthChanged(unsigned roomIndex, unsigned eid, int health);
    void onRoomChanged(unsigned nextRoomIndex, unsigned eid, GateMap   gate);
    
    ControlSystem* getCtrlSystem();
    
    lib::Subject<void(unsigned, unsigned, int)>                      onHealthChanged;
    lib::Subject<void(unsigned, unsigned, GateMap)>                  onGateTriggered;
    lib::Subject<void(unsigned, const cp::GearComponent&)>           onGearChanged;
    
private:
    
    ControlSystem controlSystem;
    
    void showRoom(unsigned roomIndex,
                  std::function<void()> after);
    void registerEvents(RoomSystemCtrl* ctrl);
    //local ecs
    lib::EcsGroup   ecsGroup;
    
    
    //room systems
    std::map<unsigned, RoomSystemCtrl*> roomSystems;
    
    //data
    FloorData*    data;
    std::map<unsigned, LayeredContainer*>      roomViews;
    std::map<unsigned, NodeRenderer*>   roomPreviews;
    
    //view
    cc::Node* view = nullptr;
    GameCamera* cam = nullptr;
    
    //event
    std::vector<lib::Registration> eventRegs;
    
    lib::Random& random;
    
    PlayerData* playerData = nullptr;
};
