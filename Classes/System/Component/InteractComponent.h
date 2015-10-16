#pragma once
#include "ProfileData.h"

struct InteractComponent
{
    using ActionType = ProfileInteractInfo::ActionType;
    using PrereqType = ProfileInteractInfo::PrereqType;
    
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
    }
    
    //input
    std::string animKeyOn;
    std::string animKeyOff;
    bool triggerActivation = false;
    
    ProfileInteractInfo::PrereqType prerequisite = PrereqType::NONE;
    ProfileInteractInfo::ActionType action = ActionType::NONE;
    
    //output
    bool activated = false;
    bool busy = false;
    bool triggeredOnce = false;
};