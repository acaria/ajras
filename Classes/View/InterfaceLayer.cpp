#include "Headers.h"

InterfaceLayer * InterfaceLayer::create()
{
    InterfaceLayer * layer = new (std::nothrow) InterfaceLayer();
    if(layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

InterfaceLayer::InterfaceLayer()
{
}

InterfaceLayer::~InterfaceLayer()
{
    this->targetEnemy->release();
    this->targetFriend->release();
}

void InterfaceLayer::setTargetID(unsigned eid, bool friendly)
{
    if (eid == this->curTargetEntityID)
        return;
    if (!ecs::has<cp::Render, cp::Collision>(eid))
    {
        Log("impossible to target without required components");
        return;
    }
    
    this->clearTarget();
    
    auto& cpRender = ecs::get<cp::Render>(eid);
    auto& cpCol = ecs::get<cp::Collision>(eid);
    
    Sprite* target = friendly ? this->targetFriend : this->targetEnemy;

    cpRender.container->addChild(target, -1);
    target->setPosition({
        cpCol.rect.getMinX() + cpCol.rect.size.width / 2 - targetEnemy->getTextureRect().size.width / 2,
        cpCol.rect.getMinY() + cpCol.rect.size.height / 2 - targetEnemy->getTextureRect().size.height / 2
    });
    
    this->curTargetEntityID = eid;
}

void InterfaceLayer::setJoystick(cc::Vec2 v)
{
    this->cursor->setPosition(
        kCursorCenter.x + kCursorCenter.x * lib::clamp(v.x, -1.0f, 1.0f) / 2,
        kCursorCenter.y + kCursorCenter.y * lib::clamp(v.y, -1.0f, 1.0f) / 2
    );
}

void InterfaceLayer::clearJoystick()
{
    this->cursor->setPosition(kCursorCenter);
}

void InterfaceLayer::clearTarget()
{
    this->targetEnemy->removeFromParentAndCleanup(false);
    this->targetFriend->removeFromParentAndCleanup(false);
    this->curTargetEntityID = 0;
}

bool InterfaceLayer::init()
{
    if (!Layer::init())
        return false;
    this->targetEnemy = Sprite::createWithSpriteFrameName("target_e.png");
    this->targetEnemy->setAnchorPoint({0,0});
    this->targetEnemy->retain();
    this->targetFriend = Sprite::createWithSpriteFrameName("target_f.png");
    this->targetFriend->setAnchorPoint({0,0});
    this->targetFriend->retain();
    
    auto borders = Scale9Sprite::createWithSpriteFrameName("main_frame.png");
    borders->setAnchorPoint({0,0});
    borders->setPosition(kCanvasRect.origin - Point(10.0f,10.0f));
    borders->cocos2d::Node::setContentSize(kCanvasRect.size + Size(20.0f,20.0f));
    this->addChild(borders);
    
    this->rArrow = Sprite::createWithSpriteFrameName("arrow_right.png");
    this->rArrow->setPosition({155,90});
    
    this->lArrow = Sprite::createWithSpriteFrameName("arrow_right.png");
    this->lArrow->setFlippedX(true);
    this->lArrow->setPosition({25,90});
    
    this->uArrow = Sprite::createWithSpriteFrameName("arrow_up.png");
    this->uArrow->setPosition({94,155});
    
    this->dArrow = Sprite::createWithSpriteFrameName("arrow_up.png");
    this->dArrow->setFlippedY(true);
    this->dArrow->setPosition({94,25});
    
    this->addChild(this->lArrow);
    this->addChild(this->rArrow);
    this->addChild(this->uArrow);
    this->addChild(this->dArrow);
    
    this->cursor = Sprite::createWithSpriteFrameName("joystick2.png");
    this->cursor->setPosition(kCursorCenter);
    this->addChild(this->cursor);

    
    return true;
}