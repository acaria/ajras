#pragma once
#include "ProfileData.h"

struct InteractComponent
{
    enum class ePrereq
    {
        NONE
    };
    
    void setProfile(ProfileData* profile)
    {
        assert(profile->interaction != nullptr);
        activated = false;
        auto interaction = profile->interaction.Value;
        animKeyOn = interaction.animKeys.first;
        animKeyOff = interaction.animKeys.second;
        prerequisite = interaction.prereqType;
        action = interaction.actionType;
        
        switch(lib::hash(interaction.triggerMode))
        {
            case lib::hash("always"):
                retriggerAble = true;
                break;
            default:
                Log("invalid trigger mode detected: %s", interaction.triggerMode.c_str());
                break;
        }
        
        switch(lib::hash(interaction.triggerAnimation))
        {
            case lib::hash("repeat"):
                repeatMode = -1;
                break;
            case lib::hash("once"):
                repeatMode = 1;
                break;
            default:
                Log("invalid trigger animation detected: %s", interaction.triggerAnimation.c_str());
                break;
        }
    }
    
    //input
    std::string animKeyOn;
    std::string animKeyOff;
    int repeatMode = 1;
    bool retriggerAble = true;
    
    //input activation
    bool triggerActivation = false;
    
    lib::Nullable<std::string> prerequisite;
    std::string action;
    
    //output
    bool activated = false;
    bool busy = false;
};