#pragma once
#include "ProfileData.h"

struct VelocityComponent
{
    struct Info
    {
        float       ratio;
        float       speed;
        cc::Vec2    direction;
        float       duration;
    };
    
    void setProfile(ProfileData* profile)
    {
        assert(profile->stats != nullptr);
        assert(profile->stats.Value.move != nullptr);
        auto move = profile->stats.Value.move.Value;
        this->set(move.speed, move.acceleration, move.deceleration);
    }
    
    void set(double speed, double accelDuration, double decelDuration)
    {
        this->move.direction = {0,0};
        this->move.ratio = 1.0;
        this->move.duration = 0;
        this->move.speed = speed;
        
        this->force.direction = {0,0};
        this->force.duration = 0;
        this->force.ratio = 1.0;
        this->force.speed = 0;
        
        this->accelDuration = accelDuration;
        this->decelDuration = decelDuration;
    }
    
    void reset()
    {
        accelFactor = 0;
        decelFactor = 0;
        velocity = {0,0};
        move.direction = {0,0};
        force.direction = {0,0};
    }
    
    void applyForce(float speed, float duration, cc::Vec2 dir)
    {
        force.speed = speed;
        force.duration = duration;
        force.direction = dir.getNormalized();
        decelFactor = 1.0;
        accelFactor = 1.0;
    }
    
    double getMoveSpeed()
    {
        return move.ratio * move.speed;
    }
    
    double getForceSpeed()
    {
        return force.ratio * force.speed;
    }
    
    //input
    double accelDuration = 1.0;
    double decelDuration = 1.0;
    
    //internal
    Info   move;
    Info   force;
    double accelFactor;
    double decelFactor;
    
    //output
    cc::Vec2 velocity;
};