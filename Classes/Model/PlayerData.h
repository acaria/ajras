#pragma once

#include "Defines.h"
#include "CoreLib.h"
#include "SlotData.h"

using KeyCode = cocos2d::EventKeyboard::KeyCode;

struct PlayerData
{
    static const unsigned ctrlIndex = 1;
    static const unsigned debugIndex = 2;
    
    ~PlayerData();
    
    //inventory
    std::list<SlotData> inventory;
    
    //stats
    unsigned maxHealth = 5;
    unsigned currentHealth = 5;
    
    //entity binding
    unsigned entityFocus = 0;
    
    //camp config
    std::string startCampKey = "home";
    
    //profiles
    std::string charProfileName;
    
    //keyboard
    CtrlKeyType ctrlKeyType;
    std::map<CtrlKeyType, std::set<KeyCode>> keysDefList;

    CtrlKeyType KeyCode2KeyType(KeyCode code);
};
