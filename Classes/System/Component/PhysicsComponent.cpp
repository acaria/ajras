#include "PhysicsComponent.h"
#include "ProfileData.h"
#include "ModelProvider.h"

bool PhysicsComponent::inputIsActive()
{
    return this->forces[ForceType::INPUT].active;
}

PhysicsComponent::PhysicsComponent()
{
    this->forces[ForceType::INPUT] = {
        .curSpeed = 0,
        .active = false,
        .maxSpeed = 0,
        .inertiaFactor = 1.0,
        .direction = {0,0},
        .duration = -1
    };
    
    this->forces[ForceType::IMPACT] = {
        .curSpeed = 0,
        .active = false,
        .maxSpeed = 0,
        .inertiaFactor = 1.0,
        .direction = {0,0},
        .duration = 0
    };
    
    this->forces[ForceType::PUSH] = {
        .curSpeed = 0,
        .active = false,
        .maxSpeed = 0,
        .inertiaFactor = 1.0,
        .direction = {0,0},
        .duration = 0
    };
}

void PhysicsComponent::setInput(const cc::Vec2& direction)
{
    this->forces[ForceType::INPUT].active = true;
    this->forces[ForceType::INPUT].direction = direction;
}

void PhysicsComponent::setPush(float maxSpeed, const cc::Vec2& direction)
{
    auto& fPush = this->forces[ForceType::PUSH];
    fPush.maxSpeed = maxSpeed;
    fPush.active = true;
    fPush.direction = direction;
}

void PhysicsComponent::setImpact(float maxSpeed, float inertia,
                                 const cc::Vec2& direction, float duration)
{
    auto& fImpact = this->forces[ForceType::IMPACT];
    fImpact.active = true;
    fImpact.maxSpeed = maxSpeed;
    fImpact.direction = direction;
    fImpact.inertiaFactor = inertia;
    fImpact.duration = duration;
}

void PhysicsComponent::addImpact(float maxSpeed, float inertia,
                                 const cc::Vec2& direction, float duration)
{
    auto& fImpact = this->forces[ForceType::IMPACT];
    if (!fImpact.active)
        setImpact(maxSpeed, inertia, direction, duration);
    else
    {
        fImpact.maxSpeed = MAX(fImpact.maxSpeed, maxSpeed);
        fImpact.direction += direction;
        fImpact.direction.normalize();
        fImpact.inertiaFactor = MAX(fImpact.inertiaFactor, inertia);
        fImpact.duration = MAX(fImpact.duration, duration);
    }
}

void PhysicsComponent::resetImpact()
{
    this->forces[ForceType::IMPACT].active = false;
    this->forces[ForceType::IMPACT].curSpeed = 0;
}

void PhysicsComponent::resetInput()
{
    this->forces[ForceType::INPUT].active = false;
    this->forces[ForceType::INPUT].curSpeed = 0;
}

void PhysicsComponent::inactiveInput()
{
    this->forces[ForceType::INPUT].active = false;
}

cc::Vec2 PhysicsComponent::getResultForce()
{
    cc::Vec2 result = {0,0};
    for(auto pair : forces)
    {
        if (pair.second.active)
            result += pair.second.direction * pair.second.curSpeed;
    }
    return result;
}

void PhysicsComponent::setProfile(ProfileData* profile)
{
    assert(profile->stats != nullptr && profile->stats->physics != nullptr);
    //collision
    auto& physics = profile->stats->physics;
    this->shape = physics->bounds;
    this->strength = physics->strength;
    this->weight = physics->weight;
    
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
    this->inertia.accSpeed = physics->acceleration;
    this->inertia.decSpeed = physics->deceleration;
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