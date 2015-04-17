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

    event::Subject<void()> onEnterBeforeTransition;
    event::Subject<void()> onEnterAfterTransition;
    event::Subject<void()> onLeaveBeforeTransition;
    event::Subject<void()> onLeaveAfterTransition;
};
