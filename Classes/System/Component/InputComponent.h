#pragma once
#include "Headers.h"

struct InputComponent
{
    //output
    bool disabled = false;
    Dir orientation;
    Dir lastOrientation;
    cc::Vec2 direction;
    
    std::list<std::function<bool(unsigned)>> predicates;
    
    void disable(float duration)
    {
        double lastTime = lib::now();
        predicates.push_back([duration, lastTime](unsigned id){
            return lib::now() - lastTime > duration;
        });
    }
    
    void setDirection(Dir orientation)
    {
        this->lastOrientation = this->orientation;
        this->orientation = orientation;
        this->direction = orientation.toVec();
    }
    
    void setDirection(cc::Vec2 direction)
    {
        this->lastOrientation = this->orientation;
        this->orientation = Dir::fromVec(direction);
        this->direction = cc::Vec2(
            lib::clamp(direction.x, -1.0f, 1.0f),
            lib::clamp(direction.y, -1.0f, 1.0f)
        );
        Log("dir=%f,%f", this->direction.x, this->direction.y);
    }
};