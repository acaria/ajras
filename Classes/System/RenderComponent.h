#pragma once
#include "Headers.h"

struct RenderComponent
{
    RenderComponent() : sprite(nullptr)
    {
    }
    
    void setFrame(const std::string &frameName, cocos2d::Node *parent, int zOrder = 0);
    void setProfile(const std::string &frameName, cocos2d::Node *parent, int zOrder = 0);
    void setAnimation(const std::string &key, int repeat,
                      std::function<void(bool)> onComplete = nullptr);
    void cancelAnimation();
    void setMoveAnimation(const unsigned &orientation, bool moving);
    
    AnimationData*      getCurAnim();
    
    cocos2d::Sprite*    sprite;

#if kDrawDebug
    cocos2d::Sprite*    collision;
    cocos2d::Sprite*    melee;
#endif
    
    //input
    ProfileData*                profile;
    int                         repeat;
    std::function<void(bool)>   onComplete = nullptr;
    
    std::string             curAnimKey;
    double                  elapsedTime;
    bool                    busy = false;

private:
    cocos2d::Sprite* initSprite(const std::string &frameName);
};