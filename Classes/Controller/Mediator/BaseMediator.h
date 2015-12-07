#pragma once

#include "Event.h"

class BaseMediator
{
public:
    virtual ~BaseMediator() {};
    
    virtual void triggerAddView(cocos2d::Scene& scene) = 0;
    
    virtual void triggerRemoveView(cocos2d::Scene& scene)
    {
        this->eventRegs.clear();
    }
    
    virtual void onTick(double dt)
    {
    }
    
    virtual void onAnimate(double dt, double tickPercent)
    {
    }

    
protected:
    std::vector<lib::Registration> eventRegs;
};
