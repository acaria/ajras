#pragma once

class ProfileData;

#include "Defines.h"
#include "Dir.h"

struct PhysicsComponent
{
    PhysicsComponent();

    struct ForceInfo
    {
        bool        active;
        float       curSpeed;
        float       maxSpeed;
        float       accSpeed;
        float       decSpeed;
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
    void addForce(float speed, float duration, cc::Vec2 dir);
    void addForce(float speed, float duration, cc::Vec2 dir,
                  float accSpeed, float decSpeed);
    
    void resetForces();

    //input/collision
    bool                    enabled;
    cc::Rect                shape;
    def::collision::Cat     category;
    
    //input/movement
    ForceInfo               movement;
    std::list<ForceInfo>    external;
    float                   weight;
    
    //output
    CollisionType           collisionState = CollisionType::NONE;
    cc::Vec2                velocity;
};