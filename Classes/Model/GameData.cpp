#include "GameData.h"
#include "ModelProvider.h"
#include "Defines.h"

void GameData::loadPlayer()
{
    if (this->playerData != nullptr)
        delete this->playerData;
    
    this->playerData = PlayerData::createDefault();
}

void GameData::loadCamp()
{
    if (this->camp != nullptr)
        delete this->camp;
    
    this->camp = CampData::load("camp1");
    assert(camp);
}

void GameData::loadMission()
{
    if (this->floor != nullptr)
        delete this->floor;

    this->floor = FloorData::generate("fd1-1");
    assert(floor);
}

void GameData::save()
{
}

void GameData::reset()
{
    ModelProvider::instance()->profile.clear();
    
    if (this->floor != nullptr)
    {
        delete floor;
        floor = nullptr;
    }
    
    if (this->playerData != nullptr)
    {
        delete playerData;
        playerData = nullptr;
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

FloorData* GameData::curFloor()
{
    return floor;
}

CampData* GameData::curCamp()
{
    return camp;
}