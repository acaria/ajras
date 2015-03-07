#pragma once
#include "Headers.h"

struct VelocityComponent
{
    void set(double speed, double accelDuration, double decelDuration)
    {
        this->speed = speed;
        this->accelDuration = accelDuration;
        this->decelDuration = decelDuration;
    }
    
    void reset()
    {
        accelFactor = 0;
        decelFactor = 0;
        velocity = {0,0};
        direction = {0,0};
    }
    
    //input
    double speed;
    double accelDuration = 1.0;
    double decelDuration = 1.0;
    
    //internal
    double          accelFactor;
    double          decelFactor;
    cocos2d::Vec2   direction;
    
    //output
    cocos2d::Vec2 velocity;
};