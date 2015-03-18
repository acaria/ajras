#pragma once
#include "Headers.h"

struct RenderComponent
{
    RenderComponent() : sprite(nullptr)
    {
        this->container = cocos2d::Sprite::create();
        this->container->setAnchorPoint({0,0});
    }
    
    void setFrame(const std::string &frameName, cocos2d::Node *parent, int zOrder = 0);
    void setProfile(const std::string &frameName, cocos2d::Node *parent, int zOrder = 0);
    void setAnimation(const std::string &key, int repeat,
                      std::function<void(bool)> onComplete = nullptr);
    void cancelAnimation();
    void setMoveAnimation(const unsigned &orientation, bool moving);
    void setMoveCategory(const std::string& cat);
    
    AnimationData*      getCurAnim();
    std::string         moveAnimationKey = "";
    
    cocos2d::Sprite*    container;
    

#if kDrawDebug
    cocos2d::Sprite*    collision;
    cocos2d::Sprite*    melee;
#endif

#if kDrawInfo
    cocos2d::Label*     lInfo;
#endif
    
    //input
    ProfileData*                profile;
    int                         repeat;
    std::function<void(bool)>   onComplete = nullptr;
    
    std::string             curAnimKey;
    double                  elapsedTime;
    bool                    busy = false;

    //sprite commands
    void setLocalZOrder(int z);
    void setPosition(const cocos2d::Vec2& position);
    void setFlippedX(bool);
    void setFlippedY(bool);
    void setSpriteFrame(const std::string &spriteFrameName);
    cocos2d::Action* runAction(cocos2d::Action* action);
private:
    cocos2d::Sprite*    sprite;
    cocos2d::Sprite* initSprite(const std::string &frameName);
};