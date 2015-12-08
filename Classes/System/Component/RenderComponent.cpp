#include "RenderComponent.h"
#include "ModelProvider.h"
#include "AnimationData.h"
#include "ProfileData.h"
#include "LayeredContainer.h"
#include "Defines.h"

RenderComponent::RenderComponent() : sprite(nullptr)
{
}

void RenderComponent::setFrame(const std::string &frameName,
                               LayeredContainer *parent,
                               def::LayerType layerType)
{
    this->moveAnimationKey = "";
    this->profile = nullptr;
    this->sprite = parent->createChild(frameName, layerType);
}

void RenderComponent::setProfile(const std::string &profileName,
                                 LayeredContainer* parent)
{
    this->setProfile(ModelProvider::instance()->profile.get(profileName), parent);
}

void RenderComponent::setProfile(ProfileData* profile, LayeredContainer* parent)
{
    this->profile = profile;
    this->setAnimation("idle", -1);
    //set default walk animation
    this->setMoveCategory("walk");
    
    cc::Vec2 zMargin {0,0};
    if (this->profile->stats != nullptr && this->profile->stats->physics != nullptr)
        zMargin = this->profile->stats->physics->bounds.origin;
    
    this->sprite = parent->createChild(getCurAnim()->frameNames.at(0),
                                       this->chooseLayer(this->profile),
                                       zMargin);
    this->busy = false;
    
    if (profile->stats != nullptr)
    {
        this->moveAnimation = profile->stats->physics != nullptr;
        this->orientationAnimation = profile->stats->orientation;
    }
}

void RenderComponent::setMoveCategory(const std::string &cat)
{
    if (this->moveAnimationKey == "_" + cat)
        return;
    if (this->profile == nullptr)
        return;
    if (this->profile->animCategoryExists(cat))
    {
        this->moveAnimationKey = cat + "_";
        return;
    }
    if (this->profile->animCategoryExists("walk"))
    {
        this->moveAnimationKey = "walk_";
        return;
    }
    this->moveAnimationKey = "";
}

void RenderComponent::cancelAnimation()
{
    if (this->onComplete != nullptr)
        onComplete(true);
    this->setAnimation("idle", -1);
    this->busy = false;
}

float RenderComponent::getAnimationDuration(const std::string &key)
{
    return this->profile->getKeyAnimation(key)->duration();
}

void RenderComponent::setAnimation(const std::string &key, int repeat, std::function<void(bool)> onComplete)
{
    this->busy = true;
    this->elapsedTime = 0.0f;
    this->repeat = repeat;
    this->curAnimKey = key;
    this->onComplete = onComplete;
}

void RenderComponent::setMoveAnimation(const Dir &orientation, bool moving)
{
    if (this->profile == nullptr)
        return;
    auto key = this->moveAnimationKey + (moving?"move":"idle");
    auto anim = this->profile->getDirAnimation(orientation, key);
    if (anim != nullptr && anim->key != this->curAnimKey)
    {
        this->elapsedTime = 0.0f;
        this->repeat = -1;
        this->curAnimKey = anim->key;
    }
    this->onComplete = nullptr;
}

def::LayerType RenderComponent::chooseLayer(ProfileData* profile)
{
    if (profile != nullptr && profile->stats != nullptr && profile->stats->physics != nullptr)
    {
        switch(lib::hash(profile->stats->physics->category))
        {
            case lib::hash("walkable"): return def::LayerType::MAIN1;
            case lib::hash("flyable"): return def::LayerType::MAIN2;
            default: return def::LayerType::FG;
        }
    }
    return def::LayerType::BG;
}

AnimationData* RenderComponent::getCurAnim()
{
    if (this->profile == nullptr)
        return nullptr;
    if (!this->curAnimKey.length())
        return nullptr;
    return this->profile->getKeyAnimation(this->curAnimKey);
}