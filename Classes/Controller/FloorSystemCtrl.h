#pragma once

class FloorData;
class PlayerData;
class NodeRenderer;
class GateMap;

#include "CtrlMissionSystem.h"
#include "RoomSystemCtrl.h"
#include "RoomLayer.h"
#include "Randgine.h"
#include "Event.h"
#include "GameCamera.h"

class FloorSystemCtrl
{
public:
    FloorSystemCtrl();
    ~FloorSystemCtrl();
    
    void load(GameCamera* cam, cc::Node* view, FloorData* data);
    void start();
    void displayDebug(cc::Node* view, FloorData* data);
    cc::Sprite* displayMap(FloorData* data);
    
    void tick(double dt);
    void animate(double dt, double tickPercent);
    
    void healthChanged(unsigned roomIndex, unsigned eid, int health);
    void onRoomChanged(unsigned nextRoomIndex, unsigned eid, GateMap   gate);
    
    CtrlMissionSystem* getCtrlSystem();
    
    lib::Subject<void(unsigned, unsigned, int)>     onHealthChanged;
    lib::Subject<void(unsigned, unsigned, GateMap)> onGateTriggered;
    
private:
    
    CtrlMissionSystem ctrlMissionSystem;
    
    void clear();
    void showRoom(unsigned roomIndex,
                  std::function<void()> after);
    void registerEvents(RoomSystemCtrl* ctrl);
    //local ecs
    lib::EcsGroup   ecsGroup;
    
    
    //room systems
    std::map<unsigned, RoomSystemCtrl*> roomSystems;
    
    //data
    FloorData*    data;
    std::map<unsigned, RoomLayer*>      roomViews;
    std::map<unsigned, NodeRenderer*>   roomPreviews;
    
    //view
    cc::Node* view = nullptr;
    GameCamera* cam = nullptr;
    
    //event
    std::vector<lib::Registration> eventRegs;
    
    unsigned currentRoomIndex;
    lib::Random& random;
    
    //focus player
    PlayerData* playerFocus;
};
