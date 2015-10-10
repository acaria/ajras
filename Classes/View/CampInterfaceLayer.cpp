#include "CampInterfaceLayer.h"

CampInterfaceLayer * CampInterfaceLayer::create()
{
    CampInterfaceLayer* layer = new (std::nothrow) CampInterfaceLayer();
    if(layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

CampInterfaceLayer::CampInterfaceLayer()
{
}

CampInterfaceLayer::~CampInterfaceLayer()
{
}

cc::Vec2 CampInterfaceLayer::setJoystick(cc::Point pos)
{
    cc::Vec2 result = {
        (pos.x - kCursorCenter.x) / kCursorRegion.x,
        (pos.y - kCursorCenter.y) / kCursorRegion.y};
    
    auto length = result.getLength();
    if (length > 1.0)
        result = result.getNormalized();
    
    this->joyStick->setScaleX((1 - MIN(1.0, length)) * 0.3 + 0.7);
    this->joyStick->setRotation(-CC_RADIANS_TO_DEGREES(result.getAngle()));
    
    this->joyStick->setPosition(
        kCursorCenter.x + kCursorRegion.x * result.x,
        kCursorCenter.y + kCursorRegion.y * result.y
    );
    
    return result;
}

void CampInterfaceLayer::clearJoystick()
{
    this->joyStick->setRotation(0);
    this->joyStick->setScaleX(1.0);
    this->joyStick->setPosition(kCursorCenter);
}

bool CampInterfaceLayer::init()
{
    if (!Layer::init())
        return false;
    
    this->joyBase = cc::Sprite::createWithSpriteFrameName("joy2.png");
    this->joyBase->setPosition(kCursorCenter);
    this->addChild(this->joyBase);
    
    this->joyStick = cc::Sprite::createWithSpriteFrameName("joy1.png");
    this->joyStick->setPosition(kCursorCenter);
    this->addChild(this->joyStick);
    
    return true;
}
