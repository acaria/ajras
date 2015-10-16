#pragma once

#include "CoreLib.h"

struct ProfileInteractInfo
{
    enum class TriggerMode
    {
        ONCE, ALWAYS
    };
    
    enum class ActionType {
        NONE, REWARD
    };
    
    enum class PrereqType {
        NONE
    };

    //ctors
    ProfileInteractInfo() {};
    ProfileInteractInfo(const cc::ValueMap& data);
    
    //properties
    PrereqType                          prereqType;
    std::pair<std::string, std::string> animKeys;
    ActionType                          actionType;
    lib::Nullable<std::string>          actionParams;
    TriggerMode                         triggerMode;
    
private:
    static std::map<std::string, ProfileInteractInfo::TriggerMode>  triggerModeMap;
    static std::map<std::string, ProfileInteractInfo::ActionType>   actionTypeMap;
    static std::map<std::string, ProfileInteractInfo::PrereqType>   prereqTypeMap;
};