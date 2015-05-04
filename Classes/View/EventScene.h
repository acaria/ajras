#pragma once

#include "Event.h"

class EventScene : public cocos2d::Scene
{
public:
    virtual ~EventScene() {}
    
    void onEnter() override
    {
        onEnterBeforeTransition();
        cocos2d::Scene::onEnter();
    }

    void onEnterTransitionDidFinish() override
    {
        onEnterAfterTransition();
        cocos2d::Scene::onEnterTransitionDidFinish();
    }

    void onExit() override
    {
        onLeaveAfterTransition();
        cocos2d::Scene::onExit();
    }

    lib::Subject<void()> onEnterBeforeTransition;
    lib::Subject<void()> onEnterAfterTransition;
    lib::Subject<void()> onLeaveBeforeTransition;
    lib::Subject<void()> onLeaveAfterTransition;
};
