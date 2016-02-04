#pragma once

#include "Defines.h"
#include "CoreLib.h"
#include "PlayerEntity.h"
#include "Dir.h"

using KeyCode = cc::EventKeyboard::KeyCode;

struct PlayerData
{
    //def
    using keysBindingMap = std::map<unsigned, std::map<CtrlKeyType, std::set<KeyCode>>>;
    
    //helpers
    static PlayerData* createDefault();
    static std::list<unsigned> getCtrlIdxList();
    
    ~PlayerData();
    
    //camp config
    std::string startCampKey = "home";
    
    //keyboard
    keysBindingMap keysDefList;
    std::pair<unsigned, CtrlKeyType> KeyCode2KeyType(KeyCode code);
    
    //entities main=0, sub>0
    std::list<PlayerEntity> entities;
    
    unsigned getEntityFocusID();
};
