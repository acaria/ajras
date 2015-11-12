#pragma once

class BehaviourData;
class ProfileData;

#include "BehaviourNodes.h"
#include "Defines.h"

struct AIComponent
{
    using state = behaviour::nState;
 
    enum class eType
    {
        NONE,
        MOOD
    };
    
    enum class eSleep
    {
        NONE, BIRD, HUMAN
    };
    
    void setProfile(ProfileData* profile);
    void setProfile(const std::string &profileName);
    
    void reset()
    {
        processing = false;
        board.reset();
    }
    
    //input
    BehaviourData* bref;
    float sightRange;
    eSleep sleep = eSleep::NONE;
    //------
    
    //internal
    behaviour::BoardNode board;
    bool processing = false;
    
    static std::map<std::string, eType>     mapType;
    static std::map<std::string, eSleep>    mapSleep;
};