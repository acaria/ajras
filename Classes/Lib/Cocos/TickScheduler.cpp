#include "TickScheduler.h"

TickScheduler::TickScheduler(std::function<void(float)> updateFunc)
{
    this->onUpdate = updateFunc;
}

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
    this->onUpdate(delta);
}