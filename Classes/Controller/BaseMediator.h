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
    
protected:
    std::vector<lib::Registration> eventRegs;
};
