#pragma once

class TickCtrl
{
public:
    TickCtrl(const std::function<void(double)>& onTick,
             const std::function<void(double, double)>& onAnimate);
    
    void update(float dt);
    void schedule(cocos2d::Node *parent);

    //accessors
    double getTimeScale();
    double getElapsedTime();
    int getFPS();
    unsigned long getTicks();
    void setTimeScale(double scale);

private:
    struct Fps {
        double timer = 0;
        int counter = 0;
        int display = 0;
    };

    void clear();
    
    std::function<void(double)> onTick;
    std::function<void(double, double)> onAnimate;
    
    double sleepTimer = 0;
    double timeScale = 1.0;
    double lastUpdateTime = 0.0;
    double accumulator = 0.0;
    double elapsedTime = 0.0;
    unsigned long currentTick = 0;
    Fps fps;
};