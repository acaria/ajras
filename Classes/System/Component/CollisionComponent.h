#pragma once
#include "Headers.h"

struct CollisionComponent
{
    void set(cocos2d::Rect rect);
    void setProfile(const std::string& profileName);
    void setProfile(ProfileData* profileData);
    
    cocos2d::Rect       rect;
    CollisionCategory   category;
    bool                collide = false;
};