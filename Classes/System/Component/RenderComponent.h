#pragma once
class Dir;
class AnimationData;
class ProfileData;

#include "Defines.h"
#include "LayeredContainer.h"

struct RenderComponent
{
    RenderComponent();

    void setFrame(const std::string &frameName,
                  LayeredContainer* parent,
                  def::LayerType layerType);

    void setProfile(const std::string &frameName,
                    LayeredContainer* parent);
    
    void setProfile(ProfileData* profile, LayeredContainer* parent);

    void setAnimation(const std::string &key, int repeat,
                      std::function<void(bool)> onComplete = nullptr);
    void cancelAnimation();
    void setMoveAnimation(const Dir &orientation, bool moving);
    void setMoveCategory(const std::string& cat);
    
    static def::LayerType chooseLayer(ProfileData* profile);
    //cc::Layer* chooseLayer(LayeredContainer* LayeredNode);
    
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
    LayeredSprite* sprite;

private:
    LayeredSprite* initSprite(LayeredSprite* sprite);
};