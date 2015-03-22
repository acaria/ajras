#include "Headers.h"

void CollisionComponent::setProfile(const std::string& profileName)
{
    auto profile = GameCtrl::instance()->profileModel.get(profileName);

    this->rect = profile->collisionRect;
    
    if (profile->collisionCat == "walkable")
        this->category = CollisionCategory::walkable;
    else if (profile->collisionCat == "flyable")
        this->category = CollisionCategory::flyable;
    else
        Log("unrecognised category:%s", profile->collisionCat.c_str());
}

void CollisionComponent::set(cocos2d::Rect rect)
{
    this->rect = rect;
    this->category = CollisionCategory::walkable;
}