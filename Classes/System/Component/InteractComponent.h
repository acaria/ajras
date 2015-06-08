#pragma once
#include "ProfileData.h"

struct InteractComponent
{
    enum class ePrereq
    {
        NONE
    };
    
    enum class eAction
    {
        NONE,
        REWARD
    };
    
    void setProfile(ProfileData* profile)
    {
        assert(profile->withInteraction);
        activated = false;
        animKeyOn = profile->interAnimKeys.first;
        animKeyOff = profile->interAnimKeys.second;
        prerequisite = prereqMap[profile->interPrereq];
        action = actionMap[profile->interActionName];
    }
    
    //input
    std::string animKeyOn;
    std::string animKeyOff;
    bool triggerActivation = false;
    
    //prerequisiste
    ePrereq prerequisite = ePrereq::NONE;
    eAction action = eAction::NONE;
    
    //output
    bool activated = false;
    bool busy = false;
    
private:
    static std::map<std::string, ePrereq> prereqMap;
    static std::map<std::string, eAction> actionMap;

};