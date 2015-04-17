#pragma once

class BehaviourData;
class ProfileData;

#include "BehaviourNodes.h"

struct AIComponent
{
    using state = behaviour::nState;
    
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
    //------
    
    behaviour::BoardNode board;
};