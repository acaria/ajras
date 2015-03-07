#pragma once
#include "Headers.h"

struct RenderComponent
{
    RenderComponent() : sprite(nullptr)
    {
    }
    
    void setFrame(const std::string &frameName, cocos2d::Node *parent, int zOrder = 0);
    void setProfile(const std::string &frameName, cocos2d::Node *parent, int zOrder = 0);
    void setAnimation(const std::string &key, int repeat);
    
    AnimationData*      getCurAnim();
    
    cocos2d::Sprite*    sprite;

#if kDrawDebug
    cocos2d::Sprite*    collision;
#endif
    
    ProfileData*        profile;
    
    std::string         curAnimKey;
    double              elapsedTime;
    int                 repeat;

private:
    cocos2d::Sprite* initSprite(const std::string &frameName);
};