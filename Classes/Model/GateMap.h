#pragma once
#include "GateInfo.h"

struct GateMap
{
    //definition
    enum class CmdType
    {
        NONE,
        CHANGE_ROOM,
        ENTER_MAP,
        EXIT_MAP
    };
    
    //model
    GateInfo    info;
    unsigned    destRoomIndex;
    unsigned    destGateIndex;
    
    //ctrl
    CmdType     cmd;
    
    //view
    std::string tileName;
};