#pragma once
#include "Headers.h"

class ProfileData
{
public:

    ProfileData(const std::string &path);
    ~ProfileData();
    
    AnimationData*  getDirAnimation(unsigned orientation, bool moving);
    AnimationData*  getKeyAnimation(const std::string& key);
    
    std::string     collisionCat;
    cocos2d::Rect   collisionRect;
    
private:
    std::map<std::string, AnimationData*> animationData;
    std::string getTagName(unsigned int orientation);
};