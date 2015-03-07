#pragma once
#include "Headers.h"

struct CollisionComponent
{
    void set(cocos2d::Rect rect)
    {
        this->rect = rect;
        this->category = CollisionCategory::walkable;
    }
    
    void setProfile(const std::string& profileName);
    
    cocos2d::Rect       rect;
    CollisionCategory   category;
};