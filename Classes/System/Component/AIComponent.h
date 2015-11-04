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
    
    double timer = 0;
    
    void reset()
    {
        board.reset();
    }
    
    //input
    BehaviourData* bref;
    float sightRange;
    eSleep sleep = eSleep::NONE;
    //------
    
    behaviour::BoardNode board;
    
    static std::map<std::string, eType>     mapType;
    static std::map<std::string, eSleep>    mapSleep;
};