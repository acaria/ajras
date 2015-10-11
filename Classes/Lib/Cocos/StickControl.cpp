#include "StickControl.h"

StickControl* StickControl::create(const std::string &bgImg, const std::string &stickImg,
                                   int area, int range){
    StickControl* stick = new (std::nothrow) StickControl(area, range);
    if(stick && stick->initWithKey(bgImg, stickImg))
    {
        stick->autorelease();
        return stick;
    }
    CC_SAFE_DELETE(stick);
    return nullptr;
}

void StickControl::setStickPosition(cc::Point position)
{
    auto factor = (position - this->getPosition()) / this->range;
    
    auto length = factor.getLength();
    if (length > 1.0)
        factor = factor.getNormalized();
    
    this->stick->setScaleX((1 - MIN(1.0, length)) * 0.3 + 0.7);
    this->stick->setRotation(-CC_RADIANS_TO_DEGREES(factor.getAngle()));
    
    this->stick->setPosition(
        range * factor.x, range * factor.y
    );
    
    this->onTrigger(factor);
}

bool StickControl::initWithKey(const std::string &bgImg, const std::string &stickImg)
{
    if (!this->init())
        return false;
    
    this->bg = cc::Sprite::createWithSpriteFrameName(bgImg);
    this->stick = cc::Sprite::createWithSpriteFrameName(stickImg);
    
    this->addChild(bg);
    this->addChild(stick);
    
    auto listener = cc::EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = [this](cc::Touch* touch, cocos2d::Event* event) {
        float distance = this->getPosition().getDistance(touch->getLocation());
        auto handled = distance < this->area;
        
        if (handled)
            this->setStickPosition(touch->getLocation());
        return handled;
    };
    
    listener->onTouchMoved = [this](cc::Touch* touch, cocos2d::Event* event) {
        this->setStickPosition(touch->getLocation());

    };
    
    listener->onTouchEnded = [this](cc::Touch* touch, cocos2d::Event* event) {
        this->stick->setRotation(0);
        this->stick->setScaleX(1.0);
        this->stick->setPosition({0,0});
        this->onRelease();
    };
    
    listener->onTouchCancelled = [this](cc::Touch* touch, cocos2d::Event* event) {
        this->stick->setRotation(0);
        this->stick->setScaleX(1.0);
        this->stick->setPosition({0,0});
        this->onRelease();
    };
    
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}
