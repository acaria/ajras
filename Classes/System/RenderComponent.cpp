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
}

cocos2d::Sprite* RenderComponent::initSprite(const std::string &frameName)
{
    auto res = cocos2d::Sprite::createWithSpriteFrameName(frameName);
    res->getTexture()->setAliasTexParameters();
    res->setAnchorPoint({0, 0});
    
#if kDrawDebug
    this->collision = Sprite::createWithSpriteFrameName("pixel.png");
    this->collision->setOpacity(100);
    this->collision->setAnchorPoint({0,0});
    this->collision->setVisible(false);
    res->addChild(this->collision);
#endif
    
    return res;
}

void RenderComponent::setAnimation(const std::string &key, int repeat)
{
    this->elapsedTime = 0.0f;
    this->repeat = repeat;
    this->curAnimKey = key;
}

AnimationData* RenderComponent::getCurAnim()
{
    if (this->profile == nullptr)
        return nullptr;
    if (!this->curAnimKey.length())
        return nullptr;
    return this->profile->getKeyAnimation(this->curAnimKey);
}