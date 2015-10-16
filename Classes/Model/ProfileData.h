#pragma once

#include "CoreLib.h"
#include "ProfileInteractInfo.h"
#include "ProfileStatsInfo.h"

class AnimationData;
class Dir;

class ProfileData
{
public:

    ProfileData(const std::string &path);
    ~ProfileData();
    
    AnimationData*  getDirAnimation(const Dir& orientation,
                                    const std::string& key);
    AnimationData*  getKeyAnimation(const std::string& key);
    
    //collision
    std::string     collisionCat;
    cocos2d::Rect   collisionRect;
    
    //behaviour
    std::string     behaviourKey;
    
    //category
    std::string     moodCategory = "neutral";
    std::string     sleepCategory = "none";
    
    lib::Nullable<ProfileInteractInfo>  interaction;
    lib::Nullable<ProfileStatsInfo>     stats;
    
    bool withCollision = false;
    bool withBehaviour = false;
    
    static std::string getTagName(const Dir& orientation);
    bool animCategoryExists(const std::string& category);
    
    std::string toString();
    
private:
    std::string path;

    //animations
    std::map<std::string, AnimationData*> animationData;
    std::vector<std::string> animationCategory;
    void extractAnims(const std::string& rootKey,
                      const cocos2d::ValueMap& data);
};