#pragma once

class ProfileData;

#include "Defines.h"

struct CollisionComponent
{
    enum CType {
        NONE,
        DECOR,
        OBJECT,
        COLLECT
    };

    void set(cc::Rect rect, CollisionCategory cat = CollisionCategory::walkable);
    void setProfile(const std::string& profileName);
    void setProfile(ProfileData* profileData);
    
    cc::Rect            rect;
    CollisionCategory   category;
    CType               current = CType::NONE;
};