#pragma once

#include "ModelFacade.h"

class PlayerData;
class FloorData;

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
    FloorData* curFloor();
    
    ModelFacade model;
    
private:
//dynamics
    PlayerData* player = nullptr;
    FloorData* floor = nullptr;
};