#pragma once

class ProfileData;

#include "Defines.h"
#include "Dir.h"

struct PhysicsComponent
{
    PhysicsComponent();

    struct ForceInfo
    {
        float       ratio;
        float       speed;
        cc::Vec2    direction;
        float       duration;
    };

    enum CollisionType {
        NONE,
        DECOR,
        OBJECT,
        COLLECT
    };

    void setProfile(const std::string& profileName);
    void setProfile(ProfileData* profileData);
    void applyForce(float speed, float duration, cc::Vec2 dir);
    
    double  getMoveSpeed();
    double  getForceSpeed();
    void    resetForce();

    //input/collision
    cc::Rect            shape;
    CollisionCategory   category;
    
    //input/velociy
    ForceInfo           move;
    ForceInfo           force;
    double              accelDuration = 1.0;
    double              decelDuration = 1.0;
    
    //output
    CollisionType       collisionState = CollisionType::NONE;
    cc::Vec2            velocity;
    
    //internal
    double              accelFactor;
    double              decelFactor;
};