#pragma once
class Dir;
class AnimationData;
class ProfileData;
class LayeredNode;

#include "Defines.h"

struct RenderComponent
{
    RenderComponent();

    void setFrame(const std::string &frameName, cc::Node *parent, int zOrder = 0);
    void setProfile(const std::string &frameName, cc::Node *parent, int zOrder = 0);
    void setProfile(ProfileData* profile, cc::Node *parent, int zOrder = 0);
    void setAnimation(const std::string &key, int repeat,
                      std::function<void(bool)> onComplete = nullptr);
    void cancelAnimation();
    void setMoveAnimation(const Dir &orientation, bool moving);
    void setMoveCategory(const std::string& cat);
    
    static cc::Layer* chooseLayer(ProfileData* profile, LayeredNode* layer);
    cc::Layer* chooseLayer(LayeredNode* LayeredNode);
    
    AnimationData*      getCurAnim();
    std::string         moveAnimationKey = "";

#if kDrawDebug
    cc::Sprite*    collision;
    cc::Sprite*    melee;
    cc::Sprite*    sight;
#endif

#if kDrawInfo
    cc::Label*     lInfo;
#endif
 
    //input
    ProfileData*                profile;
    int                         repeat;
    std::function<void(bool)>   onComplete = nullptr;
    
    std::string             curAnimKey;
    double                  elapsedTime;
    bool                    busy = false;
    bool                    manualPosMode = false;

    //output
    cocos2d::Sprite*    sprite;

private:
    cocos2d::Sprite* initSprite(const std::string &frameName);
};