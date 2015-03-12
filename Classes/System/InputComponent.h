#pragma once
#include "Headers.h"

struct InputComponent
{
    //output
    bool disabled = false;
    unsigned orientation;
    unsigned lastOrientation;
    
    std::list<std::function<bool(unsigned)>> predicates;
    
    void disable(float duration)
    {
        double lastTime = lib::now();
        predicates.push_back([duration, lastTime](unsigned id){
            return lib::now() - lastTime > duration;
        });
    }
};