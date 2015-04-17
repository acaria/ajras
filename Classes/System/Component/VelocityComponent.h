#pragma once
#include "ProfileData.h"

struct VelocityComponent
{
    void setProfile(ProfileData* profile)
    {
        this->set(profile->speed, profile->acceleration, profile->deceleration);
    }
    
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
    
    void applyVelocity(const cc::Vec2& v)
    {
        velocity = v;
        decelFactor = 1.0;
    }
    
    //input
    double speed;
    double accelDuration = 1.0;
    double decelDuration = 1.0;
    
    //internal
    double          accelFactor;
    double          decelFactor;
    cc::Vec2   direction;
    
    //output
    cc::Vec2 velocity;
};