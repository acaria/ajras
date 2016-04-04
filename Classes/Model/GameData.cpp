#include "GameData.h"
#include "ModelProvider.h"
#include "Defines.h"

void GameData::loadPlayer()
{
    this->playerAvailable = true;
    if (this->player != nullptr)
        delete this->player;
    
    this->player = PlayerData::createDefault();
}

void GameData::loadCamp()
{
    this->campAvailable = true;
    if (this->camp != nullptr)
        delete this->camp;
    
    this->camp = CampData::load("camp1");
    assert(camp);
}

void GameData::loadMission()
{
    this->floorAvailable = true;
    if (this->floor != nullptr)
        delete this->floor;

    this->floor = FloorData::generate("fd1-1");
    assert(floor);
}

void GameData::save()
{
}

FloorData*  GameData::getFloor()
{
    assert(floorAvailable);
    return floor;
}

CampData*   GameData::getCamp()
{
    assert(campAvailable);
    return camp;
}

PlayerData* GameData::getPlayer()
{
    assert(playerAvailable);
    return player;
}

void GameData::reset()
{
    ModelProvider::instance()->profile.clear();
    
    if (this->floor != nullptr)
    {
        delete floor;
        floor = nullptr;
    }
    
    if (this->player != nullptr)
    {
        delete player;
        player = nullptr;
    }
    
    if(this->camp != nullptr)
    {
        delete camp;
        camp = nullptr;
    }
}

GameData::~GameData()
{
    this->reset();
}

void GameData::toggleDebug()
{
    this->debugMode = ++debugMode % 2;
}