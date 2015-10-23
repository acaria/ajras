#include "RenderComponent.h"
#include "ModelProvider.h"
#include "AnimationData.h"
#include "ProfileData.h"
#include "LayeredNode.h"
#include "Defines.h"

RenderComponent::RenderComponent() : sprite(nullptr)
{
}

void RenderComponent::setFrame(const std::string &frameName, cc::Node *parent, int zOrder)
{
    this->moveAnimationKey = "";
    this->profile = nullptr;
    this->sprite = this->initSprite(frameName);
    parent->addChild(this->sprite, zOrder);
}

void RenderComponent::setProfile(const std::string &profileName,
                                 cc::Node *parent,
                                 int zOrder)
{
    this->setProfile(ModelProvider::instance()->profile.get(profileName), parent, zOrder);
}

void RenderComponent::setProfile(ProfileData* profile, cc::Node *parent, int zOrder)
{
    this->profile = profile;
    this->setAnimation("idle", -1);
    //set default walk animation
    this->setMoveCategory("walk");
    this->sprite = this->initSprite(getCurAnim()->frameNames.at(0));
    parent->addChild(this->sprite, zOrder);
    this->busy = false;
}

cocos2d::Sprite* RenderComponent::initSprite(const std::string &frameName)
{
    auto res = cocos2d::Sprite::createWithSpriteFrameName(frameName);
    res->setAnchorPoint({0, 0});
    

#if kDrawDebug
    this->collision = cc::Sprite::createWithSpriteFrameName("pixel.png");
    this->collision->setOpacity(100);
    this->collision->setColor(cc::Color3B::GREEN);
    this->collision->setAnchorPoint({0,0});
    this->collision->setVisible(false);
    res->addChild(this->collision);

    this->melee = cc::Sprite::createWithSpriteFrameName("pixel.png");
    this->melee->setOpacity(100);
    this->melee->setColor(cc::Color3B::YELLOW);
    this->melee->setAnchorPoint({0,0});
    this->melee->setVisible(false);
    res->addChild(this->melee);
    
    this->sight = cc::Sprite::createWithSpriteFrameName("circle.png");
    this->sight->setAnchorPoint({0.5,0.5});
    this->sight->setPosition(res->getContentSize().width / 2, res->getContentSize().height / 2);
    this->sight->setOpacity(80);
    this->sight->setColor(cc::Color3B::ORANGE);
    this->sight->setVisible(false);
    res->addChild(this->sight);
#endif

#if kDrawInfo
    this->lInfo = cc::Label::createWithTTF("", "fonts/04b03.ttf", 8);
    this->lInfo->setPosition(res->getContentSize().width / 2, res->getContentSize().height);
    res->addChild(this->lInfo);
#endif


    return res;
}

void RenderComponent::setMoveCategory(const std::string &cat)
{
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

cc::Layer* RenderComponent::chooseLayer(ProfileData* profile, LayeredNode *LayeredNode)
{
    if (profile != nullptr && profile->withCollision)
    {
        if (profile->collisionCat == "walkable")
            return LayeredNode->main;
        if (profile->collisionCat == "flyable")
            return LayeredNode->main2;
        return LayeredNode->bg;
    }
    return LayeredNode->bg;
}

cc::Layer* RenderComponent::chooseLayer(LayeredNode *LayeredNode)
{
    return chooseLayer(this->profile, LayeredNode);
}

AnimationData* RenderComponent::getCurAnim()
{
    if (this->profile == nullptr)
        return nullptr;
    if (!this->curAnimKey.length())
        return nullptr;
    return this->profile->getKeyAnimation(this->curAnimKey);
}