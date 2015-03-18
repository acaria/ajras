#pragma once
#include "Headers.h"

class ProfileData
{
public:

    ProfileData(const std::string &path);
    ~ProfileData();
    
    AnimationData*  getDirAnimation(unsigned orientation,
                                    const std::string& key);
    AnimationData*  getKeyAnimation(const std::string& key);
    
    std::string     collisionCat;
    cocos2d::Rect   collisionRect;
    
    static std::string getTagName(unsigned int orientation);
    bool animCategoryExists(const std::string& category);
    
private:
    std::map<std::string, AnimationData*> animationData;
    std::vector<std::string> animationCategory;
    void extractAnims(const std::string& rootKey,
                      const cocos2d::ValueMap& data);
};