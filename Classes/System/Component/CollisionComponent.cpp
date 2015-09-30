#include "CollisionComponent.h"
#include "ProfileData.h"
#include "GameCtrl.h"

void CollisionComponent::setProfile(ProfileData* profile)
{
    this->rect = profile->collisionRect;
    
    if (profile->collisionCat == "walkable")
        this->category = CollisionCategory::walkable;
    else if (profile->collisionCat == "flyable")
        this->category = CollisionCategory::flyable;
    else
        Log("unrecognised category:%s", profile->collisionCat.c_str());
}

void CollisionComponent::setProfile(const std::string& profileName)
{
    this->setProfile(GameCtrl::instance()->model.profile.get(profileName));
}

void CollisionComponent::set(cocos2d::Rect rect, CollisionCategory cat)
{
    this->rect = rect;
    this->category = cat;
}