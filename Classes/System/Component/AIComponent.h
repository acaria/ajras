#pragma once

class BehaviourData;
class ProfileData;

#include "BehaviourNodes.h"
#include "Defines.h"

struct AIComponent
{
    using state = behaviour::nState;
    
    void setProfile(ProfileData* profile);
    void setProfile(const std::string &profileName);
    
    void reset()
    {
        if (processing)
        {
            processing = false;
            board.reset();
        }
    }
    
    //input
    BehaviourData* bref;
    std::pair<float, float> sightRange;
    std::string sleep = "none";
    //------
    
    //internal
    behaviour::BoardNode board;
    bool processing = false;
};