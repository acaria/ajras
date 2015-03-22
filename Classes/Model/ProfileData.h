#pragma once
#include "Headers.h"

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
    std::string     behaviourMood;
    
    //sight
    double          sightRange;
    
    static std::string getTagName(const Dir& orientation);
    bool animCategoryExists(const std::string& category);
    
private:
    std::map<std::string, AnimationData*> animationData;
    std::vector<std::string> animationCategory;
    void extractAnims(const std::string& rootKey,
                      const cocos2d::ValueMap& data);
};