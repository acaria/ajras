#include "PlayerData.h"

CtrlKeyType PlayerData::KeyCode2KeyType(KeyCode code)
{
    for(auto list : keysDefList)
    {
        if (list.second.find(code) != list.second.end())
            return list.first;
    }
    return CtrlKeyType::none;
}

PlayerData::~PlayerData()
{
    for(auto& slot : inventory)
    {
        if (slot != nullptr)
        {
            delete slot;
            slot = nullptr;
        }
    }
}