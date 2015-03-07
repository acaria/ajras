#pragma once
#include "Headers.h"

class TickScheduler : public cocos2d::Node
{
public:
    TickScheduler() {}
    virtual ~TickScheduler() {}
    
    virtual void onEnterTransitionDidFinish() override;
    virtual void onExitTransitionDidStart() override;
    void update(float delta) override;
};