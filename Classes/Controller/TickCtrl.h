#pragma once

class TickCtrl
{
public:
    TickCtrl(std::function<void(double)> onTick,
             std::function<void(double, double)> onAnimate);
    
    void update(float dt);
    void schedule(cocos2d::Node *parent);

private:
    void clear();
    std::function<void(double)> onTick;
    std::function<void(double, double)> onAnimate;
    double lastUpdateTime = 0.0;
    double accumulator = 0.0;
    double upTime = 0.0;
    long currentTick = 0;
};