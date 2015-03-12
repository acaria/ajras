#pragma once
#include "Headers.h"

struct MeleeComponent
{
    enum type { SELF, DIR };
    
    void set(const std::string& name, type meleeType, unsigned range)
    {
        this->range = range;
        this->type = meleeType;
        this->name = name;
        this->coolDownDuration = 2.f;
    }

    double      lastTime = 0;
    unsigned    damage;
    
    //input
    std::string name;
    unsigned    range;
    type        type;
    float       coolDownDuration;
    
    bool isCoolDown()
    {
        Log("cool=%f", lib::now() - lastTime);
        return (lastTime == 0) || (lib::now() - lastTime > coolDownDuration);
    }
};