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
    
    FloorData*  curFloor();
    CampData*   curCamp();
    
    
    int debugMode = 0;
    void toggleDebug();
    
    PROPERTY_READ(PlayerData*, playerData, PlayerData);
    
private:
//dynamics
    FloorData*  floor = nullptr;
    CampData*   camp = nullptr;
};