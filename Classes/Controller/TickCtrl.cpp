#include "TickCtrl.h"
#include "TickScheduler.h"
#include "CoreLib.h"
#include "Defines.h"

TickCtrl::TickCtrl(std::function<void(double)> onTick,
                   std::function<void(double, double)> onAnimate) :
                   onTick(onTick),
                   onAnimate(onAnimate)
{
}

void TickCtrl::update(float dt)
{
    double currentTime = lib::now();
    
    if (lastUpdateTime == 0.0)
        lastUpdateTime = currentTime;
    
    double frameTime = currentTime - lastUpdateTime;
    this->accumulator += frameTime;
    this->upTime += frameTime;
    
    //normal motion
    float timeScale = 1.0f;
    cc::Director::getInstance()->getScheduler()->setTimeScale(timeScale);
    
    int tickCount = 0;
    while (this->accumulator > def::secondsPerTick)
    {
        accumulator -= def::secondsPerTick;
        tickCount++;
        this->currentTick++;
        
        //tick process
        if (onTick) this->onTick(def::secondsPerTick);
    }
    
    this->lastUpdateTime = currentTime;
    
    double tickPercent = cc::clampf(this->accumulator,
                                    0.0, def::secondsPerTick) / (double) def::secondsPerTick;
    
    //animate process
    if (onAnimate) this->onAnimate(frameTime, tickPercent);
}

void TickCtrl::clear()
{
    this->lastUpdateTime = 0.0;
    this->accumulator = 0.0;
    this->upTime = 0.0;
}

void TickCtrl::schedule(cocos2d::Node *parent)
{
    this->clear();
    
    auto scheduler = new TickScheduler();
    parent->addChild(scheduler);
    scheduler->release();
}