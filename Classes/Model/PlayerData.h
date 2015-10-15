#pragma once

#include "Defines.h"
#include "SlotData.h"

using KeyCode = cocos2d::EventKeyboard::KeyCode;

struct PlayerData
{
    ~PlayerData();
    
    //inventory
    std::list<SlotData*> inventory;
    
    //entity binding
    unsigned entityFocus = 0;
    
    //camp config
    std::string startCampKey = "home";
    
    //profiles
    std::string charProfileName;
    
    //control
    unsigned ctrlIndex = 0;
    
    //keyboard
    CtrlKeyType ctrlKeyType;
    std::map<CtrlKeyType, std::set<KeyCode>> keysDefList;

    CtrlKeyType KeyCode2KeyType(KeyCode code);
};
