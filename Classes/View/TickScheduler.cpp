#include "TickScheduler.h"
#include "GameCtrl.h"

void TickScheduler::onEnterTransitionDidFinish()
{
    cc::Node::onEnterTransitionDidFinish();
    cc::Director::getInstance()->getScheduler()->setTimeScale(1.0f);
    this->scheduleUpdate();
}

void TickScheduler::onExitTransitionDidStart()
{
    cc::Node::onExitTransitionDidStart();
    cc::Director::getInstance()->getScheduler()->setTimeScale(1.0f);
    this->unscheduleUpdate();
}

void TickScheduler::update(float delta)
{
    GameCtrl::instance()->tickUpdate(delta);
}