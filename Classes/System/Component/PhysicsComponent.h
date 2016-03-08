#pragma once

class ProfileData;

#include "Defines.h"
#include "Dir.h"

struct PhysicsComponent
{
    PhysicsComponent();

    //setters
    void setInput(const cc::Vec2& direction);
    void setPush(float maxSpeed, const cc::Vec2& direction);
    void setImpact(float maxSpeed, float inertia,
                   const cc::Vec2& direction, float duration);
    void addImpact(float maxSpeed, float inertia,
                   const cc::Vec2& direction, float duration);

    void resetImpact();
    void resetInput();
    void inactiveInput();

    bool inputIsActive();

    struct ForceInfo
    {
        bool        active;
        float       curSpeed;
        float       maxSpeed;
        float       inertiaFactor;
        float       decSpeed;
        cc::Vec2    direction;
        float       duration;
    };
    
    struct Inertia
    {
        float accSpeed;
        float decSpeed;
    };

    enum CollisionType {
        NONE,
        DECOR,
        OBJECT,
        COLLECT
    };
    
    enum class ForceType {
        INPUT,
        PUSH,
        IMPACT
    };

    void setProfile(const std::string& profileName);
    void setProfile(ProfileData* profileData);
    
    void resetForces();
    cc::Vec2 getResultForce();

    //input/collision
    cc::Rect                shape;
    def::collision::Cat     category;
    
    //input/movement
    std::map<ForceType, ForceInfo>  forces;
    Inertia                         inertia;
    float                           weight;
    float                           strength;
    
    //output
    CollisionType                   collisionState = CollisionType::NONE;
    cc::Vec2                        velocity;
};