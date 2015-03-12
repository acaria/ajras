#include "Headers.h"

void RenderComponent::setFrame(const std::string &frameName,
                               cocos2d::Node *parent,
                               int zOrder)
{
    this->profile = nullptr;
    this->sprite = this->initSprite(frameName);
    
    parent->addChild(this->sprite, zOrder);
}

void RenderComponent::setProfile(const std::string &profileName,
                                 cocos2d::Node *parent,
                                 int zOrder)
{
    this->profile = GameCtrl::instance()->profileModel.get(profileName);
    this->setAnimation("idle", -1);
    this->sprite = this->initSprite(getCurAnim()->frameNames.at(0));
    
    parent->addChild(this->sprite, zOrder);
    this->busy = false;
}

cocos2d::Sprite* RenderComponent::initSprite(const std::string &frameName)
{
    auto res = cocos2d::Sprite::createWithSpriteFrameName(frameName);
    res->getTexture()->setAliasTexParameters();
    res->setAnchorPoint({0, 0});
    
#if kDrawDebug
    this->collision = Sprite::createWithSpriteFrameName("pixel.png");
    this->collision->setOpacity(100);
    this->collision->setColor(Color3B::GREEN);
    this->collision->setAnchorPoint({0,0});
    this->collision->setVisible(false);
    res->addChild(this->collision);

    this->melee = Sprite::createWithSpriteFrameName("pixel.png");
    this->melee->setOpacity(100);
    this->melee->setColor(Color3B::YELLOW);
    this->melee->setAnchorPoint({0,0});
    this->melee->setVisible(false);
    res->addChild(this->melee);
#endif
    
    return res;
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

void RenderComponent::setMoveAnimation(const unsigned &orientation, bool moving)
{
    if (this->profile == nullptr)
        return;
    auto anim = this->profile->getDirAnimation(orientation, moving);
    if (anim != nullptr && anim->key != this->curAnimKey)
    {
        this->elapsedTime = 0.0f;
        this->repeat = -1;
        this->curAnimKey = anim->key;
    }
    this->onComplete = nullptr;
}

AnimationData* RenderComponent::getCurAnim()
{
    if (this->profile == nullptr)
        return nullptr;
    if (!this->curAnimKey.length())
        return nullptr;
    return this->profile->getKeyAnimation(this->curAnimKey);
}