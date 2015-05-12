#pragma once
class Dir;
class AnimationData;
class ProfileData;
class RoomLayer;

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
    void setOpacity(GLubyte opacity);
    void removeFromParentAndCleanup(bool);
    
    static cc::Layer* chooseLayer(ProfileData* profile, RoomLayer* layer);
    cc::Layer* chooseLayer(RoomLayer* roomLayer);
    
    AnimationData*      getCurAnim();
    std::string         moveAnimationKey = "";
    cc::Sprite* getContainer();

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

    //sprite commands
    void setLocalZOrder(int z);
    void setPosition(const cc::Vec2& position);
    const cc::Point& getPosition() const;
    const cc::Size getSize() const;
    void setFlippedX(bool);
    void setFlippedY(bool);
    void setSpriteFrame(const std::string &spriteFrameName);
    cocos2d::Action* runAction(cc::Action* action);
private:
    cocos2d::Sprite*    container;
    cocos2d::Sprite*    sprite;
    cocos2d::Sprite* initSprite(const std::string &frameName);
};