#pragma once

#include "CampData.h"
#include "PlayerData.h"
#include "FloorData.h"

class GameData
{
public:
    ~GameData();
    void loadPlayer();
    void loadCamp();
    void loadMission();
    void save();
    void reset();
    
    //accessors
    FloorData*  getFloor();
    CampData*   getCamp();
    PlayerData* getPlayer();
    
    int debugMode = 0;
    void toggleDebug();
    
private:
    bool playerAvailable = false;
    bool floorAvailable = false;
    bool campAvailable = false;

//dynamics
    PlayerData* player = nullptr;
    FloorData*  floor = nullptr;
    CampData*   camp = nullptr;
};