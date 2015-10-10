#include "GameData.h"
#include "Defines.h"

void GameData::loadPlayer()
{
    if (this->player != nullptr)
        delete this->player;
    
    //player --
    this->player = new PlayerData();
    this->player->ctrlIndex = 1;
    this->player->keysDefList[CtrlKeyType::left] = {KeyCode::KEY_LEFT_ARROW, KeyCode::KEY_A};
    this->player->keysDefList[CtrlKeyType::right] = {KeyCode::KEY_RIGHT_ARROW, KeyCode::KEY_D};
    this->player->keysDefList[CtrlKeyType::up] = {KeyCode::KEY_UP_ARROW, KeyCode::KEY_W};
    this->player->keysDefList[CtrlKeyType::down] = {KeyCode::KEY_DOWN_ARROW, KeyCode::KEY_S};
    this->player->keysDefList[CtrlKeyType::autoselect] = {KeyCode::KEY_SPACE};
    this->player->keysDefList[CtrlKeyType::sel1] = {KeyCode::KEY_1};
    this->player->keysDefList[CtrlKeyType::sel2] = {KeyCode::KEY_2};
    this->player->keysDefList[CtrlKeyType::sel3] = {KeyCode::KEY_3};
    
    //player inventory
    unsigned invCounter = 1;
    this->player->inventory.push_back(new SlotData {
        .category = ColCat::head,
        .order = invCounter++,
        .content = nullptr
    });
    this->player->inventory.push_back(new SlotData {
        .category = ColCat::object,
        .order = invCounter++,
        .content = nullptr
    });
    this->player->inventory.push_back(new SlotData {
        .category = ColCat::object,
        .order = invCounter++,
        .content = nullptr
    });
    this->player->inventory.push_back(new SlotData {
        .category = ColCat::object,
        .order = invCounter++,
        .content = nullptr
    });
    this->player->inventory.push_back(new SlotData {
        .category = ColCat::object,
        .order = invCounter++,
        .content = nullptr
    });
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
    model.profile.clear();
    
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

PlayerData* GameData::curPlayer()
{
    return player;
}

FloorData* GameData::curFloor()
{
    return floor;
}

CampData* GameData::curCamp()
{
    return camp;
}