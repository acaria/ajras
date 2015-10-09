#pragma once

class FloorData;
class PlayerData;
class NodeRenderer;
class GateMap;

#include "ControlSystem.h"
#include "RoomSystemCtrl.h"
#include "RoomLayer.h"
#include "Randgine.h"

class FloorSystemCtrl
{
public:
    FloorSystemCtrl();
    ~FloorSystemCtrl();
    
    void load(MissionScene* view, FloorData* data);
    void start();
    void displayDebug(MissionScene* view, FloorData* data);
    cc::Sprite* displayMap(FloorData* data);
    
    void tick(double dt);
    void animate(double dt, double tickPercent);
    
    void onHealthChanged(unsigned roomIndex, unsigned eid, int health);
    void onRoomChanged(unsigned nextRoomIndex, unsigned eid, GateMap   gate);
    void onFloorStart();
    void onFloorFinish();
    
private:
    
    void clear();
    void showRoom(unsigned roomIndex,
                  std::function<void()> after);
    void registerEvents(RoomSystemCtrl* ctrl);
    //local ecs
    lib::EcsGroup   ecsGroup;
    
    //floor systems
    ControlSystem   controlSystem;
    
    //room systems
    std::map<unsigned, RoomSystemCtrl*> roomSystems;
    
    //data
    FloorData*    data;
    std::map<unsigned, RoomLayer*>      roomViews;
    std::map<unsigned, NodeRenderer*>   roomPreviews;
    
    //view
    MissionScene* gView = nullptr;
    
    //event
    std::vector<lib::Registration> eventRegs;
    
    unsigned currentRoomIndex;
    lib::Random& random;
    
    //focus player
    PlayerData* playerFocus;
};
