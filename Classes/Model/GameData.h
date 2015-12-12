#pragma once

#include "CampData.h"
#include "PlayerData.h"
#include "FloorData.h"

class GameData
{
public:
    ~GameData();
    void loadShaders(); 
    void loadPlayer();
    void loadCamp();
    void loadMission();
    void save();
    void reset();
    
    PlayerData* curPlayer();
    FloorData*  curFloor();
    CampData*   curCamp();
    
    
    int debugMode = 0;
    void toggleDebug();
    
private:
//dynamics
    PlayerData* player = nullptr;
    FloorData*  floor = nullptr;
    CampData*   camp = nullptr;
};