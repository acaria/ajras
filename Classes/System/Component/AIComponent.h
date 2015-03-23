#pragma once
#include "Headers.h"

struct AIComponent
{
    using state = behaviour::nState;
    
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