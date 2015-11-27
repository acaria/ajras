#include "PhysicsComponent.h"
#include "ProfileData.h"
#include "ModelProvider.h"

PhysicsComponent::PhysicsComponent()
{
    this->enabled = true;
    
    //init movement force
    this->movement = {
        .curSpeed = 0,
        .active = false,
        .maxSpeed = 0,
        .accSpeed = 0,
        .decSpeed = 0,
        .direction = {0,0},
        .duration = 0
    };
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
        this->movement.accSpeed = move.acceleration;
        this->movement.decSpeed = move.deceleration;
        this->movement.maxSpeed = move.speed;
    }
}

void PhysicsComponent::setProfile(const std::string& profileName)
{
    this->setProfile(ModelProvider::instance()->profile.get(profileName));
}

void PhysicsComponent::addForce(float speed, float duration, cc::Vec2 dir)
{
    addForce(speed, duration, dir, movement.accSpeed, movement.decSpeed);
}

void PhysicsComponent::addForce(float speed, float duration, cc::Vec2 dir, float acc, float dec)
{
    this->external.push_back({
        .active = true,
        .curSpeed = 0,
        .maxSpeed = speed,
        .direction = dir.getNormalized(),
        .duration = duration,
        .accSpeed = acc,
        .decSpeed = dec
    });
}

void PhysicsComponent::resetForces()
{
    velocity = {0,0};
    movement.active = false;
    movement.curSpeed = 0;
    movement.direction = {0,0};
    external.clear();
}