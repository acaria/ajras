#pragma once

class MapData;

#include "ControlSystem.h"
#include "RoomSystemCtrl.h"
#include "RoomLayer.h"

class FloorSystemCtrl
{
public:
    FloorSystemCtrl() : controlSystem(ecsGroup)
    {
    }
    
    ~FloorSystemCtrl();
    
    void load(GameScene* view, MapData* data);
    void displayDebug(GameScene* view, MapData* data);
    
    RoomData* changeRoom(unsigned roomIndex, unsigned gateIndex, const std::vector<unsigned>& eids);
    
    void tick(double dt);
    void animate(double dt, double tickPercent);
    
private:
    
    void clear();
    
    //local ecs
    lib::EcsGroup   ecsGroup;
    
    //floor systems
    ControlSystem   controlSystem;
    
    //room systems
    std::map<unsigned, RoomSystemCtrl*> roomSystems;
    
    //data
    MapData*    data;
    std::map<unsigned, RoomLayer*>      roomViews;
    
    //view
    GameScene* gView = nullptr;
    
    unsigned currentRoomIndex;
};
