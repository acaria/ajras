#include "Headers.h"

void RenderComponent::setFrame(const std::string &frameName,
                               cocos2d::Node *parent,
                               int zOrder)
{
    this->moveAnimationKey = "";
    this->profile = nullptr;
    this->sprite = this->initSprite(frameName);
    this->container->addChild(this->sprite);
    
    parent->addChild(this->container, zOrder);
}

void RenderComponent::setProfile(const std::string &profileName,
                                 cocos2d::Node *parent,
                                 int zOrder)
{
    this->setProfile(GameCtrl::instance()->profileModel.get(profileName), parent, zOrder);
}

void RenderComponent::setProfile(ProfileData* profile,
                                 cocos2d::Node *parent,
                                 int zOrder)
{
    this->profile = profile;
    this->setAnimation("idle", -1);
    //set default walk animation
    this->setMoveCategory("walk");
    this->sprite = this->initSprite(getCurAnim()->frameNames.at(0));
    this->container->addChild(this->sprite);
    parent->addChild(this->container, zOrder);
    this->busy = false;
}

cocos2d::Sprite* RenderComponent::initSprite(const std::string &frameName)
{
    auto res = cocos2d::Sprite::createWithSpriteFrameName(frameName);
    res->setAnchorPoint({0, 0});
    

#if kDrawDebug
    this->collision = Sprite::createWithSpriteFrameName("pixel.png");
    this->collision->setOpacity(100);
    this->collision->setColor(Color3B::GREEN);
    this->collision->setAnchorPoint({0,0});
    this->collision->setVisible(false);
    this->container->addChild(this->collision);

    this->melee = Sprite::createWithSpriteFrameName("pixel.png");
    this->melee->setOpacity(100);
    this->melee->setColor(Color3B::YELLOW);
    this->melee->setAnchorPoint({0,0});
    this->melee->setVisible(false);
    this->container->addChild(this->melee);
    
    this->sight = Sprite::createWithSpriteFrameName("circle.png");
    this->sight->setAnchorPoint({0.5,0.5});
    this->sight->setPosition(res->getContentSize().width / 2, res->getContentSize().height / 2);
    this->sight->setOpacity(80);
    this->sight->setColor(Color3B::ORANGE);
    this->sight->setVisible(false);
    this->container->addChild(this->sight);
#endif

#if kDrawInfo
    this->lInfo = Label::createWithTTF("", "fonts/04b03.ttf", 8);
    this->lInfo->setPosition(res->getContentSize().width / 2, res->getContentSize().height);
    this->container->addChild(this->lInfo);
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

void RenderComponent::setLocalZOrder(int z)
{
    this->container->setLocalZOrder(z);
}

void RenderComponent::setPosition(const cocos2d::Vec2 &position)
{
    this->container->setPosition(position);
}

void RenderComponent::setFlippedX(bool value)
{
    this->sprite->setFlippedX(value);
}

void RenderComponent::setFlippedY(bool value)
{
    this->sprite->setFlippedY(value);
}

void RenderComponent::setSpriteFrame(const std::string &spriteFrameName)
{
    this->sprite->setSpriteFrame(spriteFrameName);
}

cocos2d::Action* RenderComponent::runAction(cocos2d::Action *action)
{
    return this->sprite->runAction(action);
}

AnimationData* RenderComponent::getCurAnim()
{
    if (this->profile == nullptr)
        return nullptr;
    if (!this->curAnimKey.length())
        return nullptr;
    return this->profile->getKeyAnimation(this->curAnimKey);
}