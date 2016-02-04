#include "PlayerData.h"

std::pair<unsigned, CtrlKeyType> PlayerData::KeyCode2KeyType(KeyCode code)
{
    for(auto el : this->keysDefList)
    {
        for(auto list : el.second)
        {
            if (list.second.find(code) != list.second.end())
                return {el.first, list.first};
        }
    }
    return {0, CtrlKeyType::none};
}

PlayerData::~PlayerData()
{
}

PlayerData* PlayerData::createDefault()
{
    auto result = new PlayerData();
    
    //binding controls
    result->keysDefList = {
        { def::CTRL1, {
            {CtrlKeyType::left,         {KeyCode::KEY_LEFT_ARROW, KeyCode::KEY_A}},
            {CtrlKeyType::right,        {KeyCode::KEY_RIGHT_ARROW, KeyCode::KEY_D}},
            {CtrlKeyType::up,           {KeyCode::KEY_UP_ARROW, KeyCode::KEY_W}},
            {CtrlKeyType::down,         {KeyCode::KEY_DOWN_ARROW, KeyCode::KEY_S}},
            {CtrlKeyType::autoselect,   {KeyCode::KEY_SPACE}},
            {CtrlKeyType::sel1,         {KeyCode::KEY_1}},
            {CtrlKeyType::sel2,         {KeyCode::KEY_2}},
            {CtrlKeyType::sel3,         {KeyCode::KEY_3}},
        }},
        { def::CTRL2, {
            {CtrlKeyType::left,         {KeyCode::KEY_J}},
            {CtrlKeyType::right,        {KeyCode::KEY_L}},
            {CtrlKeyType::up,           {KeyCode::KEY_I}},
            {CtrlKeyType::down,         {KeyCode::KEY_K}},
        }}
    };
    
    //team: leader
    
    PlayerEntity entity {
        .profileName = "boy2",
        .ctrlIndex = def::CTRL1,
        .inventory = {
            {.quantity = 0, .content = nullptr},
            {.quantity = 0, .content = nullptr},
            {.quantity = 0, .content = nullptr},
            {.quantity = 0, .content = nullptr},
            {.quantity = 0, .content = nullptr}
        }
    };
    result->entities.push_back(entity);
    
    //troops
    
    
    return result;
}

unsigned PlayerData::getEntityFocusID()
{
    if (this->entities.size() > 0)
        return this->entities.front().entityID;
    return 0;
}

std::list<unsigned> PlayerData::getCtrlIdxList()
{
    return {def::CTRL1, def::CTRL2};
}