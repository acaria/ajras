#include "LayeredSprite.h"
#include "LayeredContainer.h"

LayeredSprite* LayeredSprite::create(const std::string& frameName,
                                     LayeredContainer* container,
                                     cc::Point zMargin)
{
    auto sprite = LayeredSprite::createWithSpriteFrameName(frameName);
    sprite->container = container;
    sprite->zMargin = zMargin;
    
    return sprite;
}

LayeredSprite* LayeredSprite::createWithSpriteFrame(cc::SpriteFrame *spriteFrame)
{
    LayeredSprite *sprite = new (std::nothrow) LayeredSprite();
    if (sprite && spriteFrame && sprite->initWithSpriteFrame(spriteFrame))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

LayeredSprite* LayeredSprite::createWithSpriteFrameName(const std::string& spriteFrameName)
{
    cc::SpriteFrame *frame = cc::SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
    
#if COCOS2D_DEBUG > 0
    char msg[256] = {0};
    sprintf(msg, "Invalid spriteFrameName: %s", spriteFrameName.c_str());
    CCASSERT(frame != nullptr, msg);
#endif
    
    return createWithSpriteFrame(frame);
}

void LayeredSprite::setPosition(const cc::Vec2 &pos)
{
    LayeredSprite::setPosition(pos.x, pos.y);
}

void LayeredSprite::setPosition(float x, float y)
{
    this->actualPosition.x = x;
    this->actualPosition.y = y;
    
    float scale = cc::Director::getInstance()->getContentScaleFactor();
    if (scale > 0.0)
    {
        x = round(x * scale) / scale;
        y = round(y * scale) / scale;
    }

    Sprite::setPosition(x, y);
    this->setLocalZOrder(this->container->getZOrder(
        {x + zMargin.x, y + zMargin.y}));
}

const cc::Vec2& LayeredSprite::getPosition() const
{
    return this->actualPosition;
}

cc::Vec3 LayeredSprite::getPosition3D() const
{
    return cc::Vec3(this->actualPosition.x, this->actualPosition.y, _positionZ);
}

void LayeredSprite::switchContainer(LayeredContainer* container)
{
    assert(this->container != nullptr && this->getParent() != nullptr);
    def::LayerType type = static_cast<def::LayerType>(this->getParent()->getTag());
    this->container = container;
    this->switchLayer(type);
}

void LayeredSprite::switchLayer(def::LayerType type)
{
    this->removeFromParentAndCleanup(false);
    this->container->get(type)->addChild(this);
}

LayeredContainer* LayeredSprite::getContainer()
{
    return this->container;
}