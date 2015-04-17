#pragma once

class ProfileData;

#include "Defines.h"

struct CollisionComponent
{
    void set(cc::Rect rect);
    void setProfile(const std::string& profileName);
    void setProfile(ProfileData* profileData);
    
    cc::Rect       rect;
    CollisionCategory   category;
    bool                collide = false;
};