#include "PhysicsComponent.h"
#include "ProfileData.h"
#include "ModelProvider.h"

PhysicsComponent::PhysicsComponent()
{
    //velocity
    this->accelDuration = 0;
    this->decelDuration = 0;
    
    this->move = {
        .ratio = 1.0,
        .speed = 0,
        .direction = {0,0},
        .duration = 0
    };
    
    this->force = {
        .ratio = 1.0,
        .speed = 0,
        .direction = {0,0},
        .duration = 0
    };
}

void PhysicsComponent::setProfile(ProfileData* profile)
{
    //collision
    this->shape = profile->collisionRect;
    
    if (profile->collisionCat == "walkable")
        this->category = CollisionCategory::walkable;
    else if (profile->collisionCat == "flyable")
        this->category = CollisionCategory::flyable;
    else
        Log("unrecognised category:%s", profile->collisionCat.c_str());

    //velocity
    if (profile->stats != nullptr && profile->stats->move != nullptr)
    {
        auto move = profile->stats->move.Value;
        this->accelDuration = move.acceleration;
        this->decelDuration = move.deceleration;
        this->move.speed = move.speed;
    }
}

void PhysicsComponent::setProfile(const std::string& profileName)
{
    this->setProfile(ModelProvider::instance()->profile.get(profileName));
}

void PhysicsComponent::applyForce(float speed, float duration, cc::Vec2 dir)
{
    force.speed = speed;
    force.duration = duration;
    force.direction = dir.getNormalized();
    decelFactor = 1.0;
    accelFactor = 1.0;
}

double PhysicsComponent::getMoveSpeed()
{
    return move.ratio * move.speed;
}

double PhysicsComponent::getForceSpeed()
{
    return force.ratio * force.speed;
}

void PhysicsComponent::resetForce()
{
    accelFactor = 0;
    decelFactor = 0;
    velocity = {0,0};
    move.direction = {0,0};
    force.direction = {0,0};
}