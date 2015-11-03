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
    float getAnimationDuration(const std::string& key);
    void cancelAnimation();
    void setMoveAnimation(const Dir &orientation, bool moving);
    void setMoveCategory(const std::string& cat);
    
    static def::LayerType chooseLayer(ProfileData* profile);
    
    AnimationData*      getCurAnim();
    std::string         moveAnimationKey = "";
 
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
};