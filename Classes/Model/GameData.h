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
    
    PlayerData* curPlayer();
    FloorData*  curFloor();
    CampData*   curCamp();
    
private:
//dynamics
    PlayerData* player = nullptr;
    FloorData*  floor = nullptr;
    CampData*   camp = nullptr;
};