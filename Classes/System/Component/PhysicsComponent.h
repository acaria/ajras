#pragma once

class ProfileData;

#include "Defines.h"
#include "Dir.h"

struct PhysicsComponent
{
    PhysicsComponent();

    struct ForceInfo
    {
        void set(float maxSpeed, std::pair<float, float> inertia,
                 cc::Vec2 direction, float duration)
        {
            this->active = true;
            this->maxSpeed = maxSpeed;
            this->accSpeed = inertia.first;
            this->decSpeed = inertia.second;
            this->direction = direction;
            this->duration = duration;
        }

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
    
    enum class ForceType {
        INPUT,
        PUSH,
        IMPACT
    };

    void setProfile(const std::string& profileName);
    void setProfile(ProfileData* profileData);
    
    void resetForces();
    cc::Vec2 getResultForce();

    ForceInfo& fInput();
    ForceInfo& fImpact();
    ForceInfo& fPush();

    //input/collision
    cc::Rect                shape;
    def::collision::Cat     category;
    
    //input/movement
    std::map<ForceType, ForceInfo>  forces;
    float                           weight;
    float                           strength;
    
    //output
    CollisionType                   collisionState = CollisionType::NONE;
    cc::Vec2                        velocity;
};