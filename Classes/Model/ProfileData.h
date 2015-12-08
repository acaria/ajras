#pragma once

#include "CoreLib.h"
#include "ProfileInteractInfo.h"
#include "ProfileStatsInfo.h"
#include "ProfileBehaviourInfo.h"
#include "Defines.h"

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
    
    lib::Nullable<ProfileInteractInfo>  interaction;
    lib::Nullable<ProfileStatsInfo>     stats;
    lib::Nullable<ProfileBehaviourInfo> behaviour;
    
    static std::string getTagName(const Dir& orientation);
    bool animCategoryExists(const std::string& category);
    
    def::mood::Flags getMood();
    
private:
    std::string path;

    //animations
    std::map<std::string, AnimationData*> animationData;
    std::vector<std::string> animationCategory;
    void extractAnims(const std::string& rootKey,
                      const cocos2d::ValueMap& data);
};