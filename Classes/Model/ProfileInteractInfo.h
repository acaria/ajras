#pragma once

#include "CoreLib.h"

struct ProfileInteractInfo
{
    //ctors
    ProfileInteractInfo() {};
    ProfileInteractInfo(const cc::ValueMap& data);
    
    //properties
    lib::Nullable<std::string>          prereqType;
    std::pair<std::string, std::string> animKeys;
    std::string                         actionType;
    lib::Nullable<std::string>          actionParams;
    std::string                         triggerMode;
    std::string                         triggerAnimation;
    bool                                activated;
};