#include "Headers.h"

void TickScheduler::onEnterTransitionDidFinish()
{
    cocos2d::Node::onEnterTransitionDidFinish();
    cocos2d::Director::getInstance()->getScheduler()->setTimeScale(1.0f);
    this->scheduleUpdate();
}

void TickScheduler::onExitTransitionDidStart()
{
    cocos2d::Node::onExitTransitionDidStart();
    cocos2d::Director::getInstance()->getScheduler()->setTimeScale(1.0f);
    this->unscheduleUpdate();
}

void TickScheduler::update(float delta)
{
    GameCtrl::instance()->tickUpdate(delta);
}