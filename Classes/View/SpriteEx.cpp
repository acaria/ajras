#include "SpriteEx.h"
#include "CoreLib.h"
#include "LayeredContainer.h"

void SpriteEx::setContainer(LayeredContainer *container, cc::Point zMargin)
{
    this->container = container;
    this->zMargin = zMargin;
}

void SpriteEx::setPosition(const cc::Vec2 &pos)
{
    SpriteEx::setPosition(pos.x, pos.y);
}

bool SpriteEx::initWithTexture(cc::Texture2D *texture, const cc::Rect &rect, bool _rotated)
{
    bool result = Base::initWithTexture(texture, rect, _rotated);
    this->setAnchorPoint({0,0});
    return result;
}

void SpriteEx::setPosition(float x, float y)
{
    this->actualPosition.x = x;
    this->actualPosition.y = y;
    
    float scale = cc::Director::getInstance()->getContentScaleFactor();
    if (scale > 0.0)
    {
        x = round(x * scale) / scale;
        y = round(y * scale) / scale;
    }
    
    Base::setPosition(x, y);
 
    if (this->container != nullptr)
    {
        this->setLocalZOrder(this->container->getZOrder({x + zMargin.x, y + zMargin.y}));
    }
}

const cc::Vec2& SpriteEx::getPosition() const
{
    return this->actualPosition;
}

cc::Vec3 SpriteEx::getPosition3D() const
{
    return cc::Vec3(this->actualPosition.x, this->actualPosition.y, this->_positionZ);
}
