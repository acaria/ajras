#include "TickCtrl.h"
#include "TickScheduler.h"
#include "CoreLib.h"
#include "Defines.h"

TickCtrl::TickCtrl(const std::function<void(double)>& onTick,
                   const std::function<void(double, double)>& onAnimate) :
                   onTick(onTick),
                   onAnimate(onAnimate)
{
}

void TickCtrl::update(float dt)
{
    double currentTime = lib::now();
    
    if (lastUpdateTime == 0.0)
        lastUpdateTime = currentTime;
    
    double frameTime = (currentTime - lastUpdateTime);
    cc::Director::getInstance()->getScheduler()->setTimeScale(this->timeScale);
    
    fps.counter++;
    fps.timer += frameTime;
    if (fps.timer >= 1.0)
    {
        fps.display = fps.counter;
        fps.timer -= 1.0;
        fps.counter = 0;
    }
    frameTime *= this->timeScale;
    
    if (sleepTimer > 0.0)
    {
        sleepTimer -= frameTime;
        if (sleepTimer <= 0)
            sleepTimer = 0.0;
        else
        {
            cc::Director::getInstance()->getScheduler()->setTimeScale(0);
            return;
        }
    }
    
    this->accumulator += frameTime;
    this->upTime += frameTime;
    
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
    this->currentTick = 0;
    this->fps.counter = 0;
    this->fps.display = 0;
    this->fps.timer = 0.0;
}

void TickCtrl::schedule(cocos2d::Node *parent)
{
    using namespace std::placeholders;
    
    this->clear();
    
    auto scheduler = new TickScheduler(std::bind(&TickCtrl::update, this, _1));
    parent->addChild(scheduler);
    scheduler->release();
}

int TickCtrl::getFPS()
{
    return fps.display;
}

unsigned long TickCtrl::getTicks()
{
    return this->currentTick;
}

double TickCtrl::getTimeScale()
{
    return this->timeScale;
}

void TickCtrl::setTimeScale(double scale)
{
    this->timeScale = scale;
}
