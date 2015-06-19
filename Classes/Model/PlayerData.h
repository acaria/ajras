#pragma once

#include "Defines.h"

using KeyCode = cocos2d::EventKeyboard::KeyCode;

struct PlayerData
{
    //inventory
    unsigned nbSlots = 7;
    
    //entity binding
    unsigned entityFocus = 0;
    
    //control
    unsigned ctrlIndex = 0;
    
    //keyboard
    CtrlKeyType ctrlKeyType;
    std::map<CtrlKeyType, std::set<KeyCode>> keysDefList;

    CtrlKeyType KeyCode2KeyType(KeyCode code);
};
