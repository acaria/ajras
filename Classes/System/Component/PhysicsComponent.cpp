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
    assert(profile->stats != nullptr && profile->stats->physics != nullptr);
    //collision
    auto& physics = profile->stats->physics;
    this->shape = physics->bounds;
    
    switch(lib::hash(physics->category))
    {
        case lib::hash("walkable"):
            this->category = def::collision::Cat::walkable;
            break;
        case lib::hash("flyable"):
            this->category = def::collision::Cat::flyable;
            break;
        default:
            Log("unrecognised category:%s", physics->category.c_str());
            break;
    }

    //movement
    auto& fMove = this->forces[ForceType::INPUT];
    fMove.maxSpeed = physics->speed;
    fMove.accSpeed = physics->acceleration;
    fMove.decSpeed = physics->deceleration;
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