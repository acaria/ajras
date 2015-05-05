#pragma once

class MapData;
class NodeRenderer;
class GateMap;

#include "ControlSystem.h"
#include "RoomSystemCtrl.h"
#include "RoomLayer.h"
#include "Randgine.h"

class FloorSystemCtrl
{
public:
    FloorSystemCtrl() : controlSystem(ecsGroup),
                        random(Randgine::instance()->get(Randgine::MAP))
    {
    }
    
    ~FloorSystemCtrl();
    
    void load(GameScene* view, MapData* data);
    void start();
    void displayDebug(GameScene* view, MapData* data);
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
    GameScene* gView = nullptr;
    
    //event
    std::vector<lib::Registration> eventRegs;
    
    //mainEntity
    unsigned focusEntity = 0;
    
    unsigned currentRoomIndex;
    lib::Random& random;
};
