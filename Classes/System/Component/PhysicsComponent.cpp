#include "PhysicsComponent.h"
#include "ProfileData.h"
#include "ModelProvider.h"

PhysicsComponent::PhysicsComponent()
{
    this->forces[ForceType::INPUT] = {
        .curSpeed = 0,
        .active = false,
        .maxSpeed = 0,
        .accSpeed = 0,
        .decSpeed = 0,
        .direction = {0,0},
        .duration = -1
    };
    
    this->forces[ForceType::IMPACT] = {
        .curSpeed = 0,
        .active = false,
        .maxSpeed = 0,
        .accSpeed = 0,
        .decSpeed = 0,
        .direction = {0,0},
        .duration = 0
    };
    
    this->forces[ForceType::PUSH] = {
        .curSpeed = 0,
        .active = false,
        .maxSpeed = 0,
        .accSpeed = 0,
        .decSpeed = 0,
        .direction = {0,0},
        .duration = 0
    };
}

PhysicsComponent::ForceInfo& PhysicsComponent::fInput()
{
    return this->forces[ForceType::INPUT];
}

PhysicsComponent::ForceInfo& PhysicsComponent::fImpact()
{
    return this->forces[ForceType::IMPACT];
}

PhysicsComponent::ForceInfo& PhysicsComponent::fPush()
{
    return this->forces[ForceType::PUSH];
}

void PhysicsComponent::setProfile(ProfileData* profile)
{
    //collision
    this->shape = profile->collisionRect;
    
    if (profile->collisionCat == "walkable")
        this->category = def::collision::Cat::walkable;
    else if (profile->collisionCat == "flyable")
        this->category = def::collision::Cat::flyable;
    else
        Log("unrecognised category:%s", profile->collisionCat.c_str());

    //velocity
    if (profile->stats != nullptr && profile->stats->move != nullptr)
    {
        auto move = profile->stats->move.Value;
        auto& fMove = this->forces[ForceType::INPUT];
        fMove.maxSpeed = move.speed;
        fMove.accSpeed = move.acceleration;
        fMove.decSpeed = move.deceleration;
    }
}

void PhysicsComponent::setProfile(const std::string& profileName)
{
    this->setProfile(ModelProvider::instance()->profile.get(profileName));
}

void PhysicsComponent::resetForces()
{
    for (auto& pair : forces)
    {
        pair.second.active = false;
        pair.second.curSpeed = 0;
    }
}