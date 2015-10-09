#pragma once

class MapData;
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
    
    void load(MissionScene* view, MapData* data);
    void start();
    void displayDebug(MissionScene* view, MapData* data);
    cc::Sprite* displayMap(MapData* data);
    
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
    MapData*    data;
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
