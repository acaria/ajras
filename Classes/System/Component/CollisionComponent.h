#pragma once

class ProfileData;

struct CollisionComponent
{
    void set(cc::Rect rect);
    void setProfile(const std::string& profileName);
    void setProfile(ProfileData* profileData);
    
    cc::Rect       rect;
    CollisionCategory   category;
    bool                collide = false;
};