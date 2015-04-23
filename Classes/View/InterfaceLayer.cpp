#include "InterfaceLayer.h"
#include "Components.h"
#include "HealthBar.h"

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

HealthBar* InterfaceLayer::getHealthBar()
{
    return this->healthBar;
}

void InterfaceLayer::setTargetID(unsigned eid, bool friendly, cc::Sprite* container, cc::Point pos)
{
    this->targetEnemy->removeFromParentAndCleanup(false);
    this->targetFriend->removeFromParentAndCleanup(false);
    this->curTargetEntityID = 0;
    
    cc::Sprite* target = friendly ? this->targetFriend : this->targetEnemy;
    container->addChild(target, -1);
    target->setPosition({
        pos.x - targetEnemy->getTextureRect().size.width / 2,
        pos.y - targetEnemy->getTextureRect().size.height / 2
    });
    
    if (this->currentAction == ActionMode::melee)
    {
        auto sprite = this->getSpriteAction(currentAction);
        if (sprite != actionTargetSword)
            this->internalTransitionAction(sprite, this->actionTargetSword);
    }
    
    this->curTargetEntityID = eid;
}

void InterfaceLayer::unsetTargetID(unsigned int eid)
{
    if (eid == this->curTargetEntityID)
    {
        this->clearTarget();
    }
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

cc::Rect InterfaceLayer::getActionBounds()
{
    return cc::Rect(this->actionSelection->getPosition().x,
                    this->actionSelection->getPosition().y,
                    this->actionSelection->getContentSize().width,
                    this->actionSelection->getContentSize().height);
}

void InterfaceLayer::clearTarget()
{
    if (this->currentAction == ActionMode::melee)
    {
        auto sprite = this->getSpriteAction(currentAction);
        if (sprite == actionTargetSword)
            this->internalTransitionAction(sprite, this->actionSword);
    }
    this->targetEnemy->removeFromParentAndCleanup(false);
    this->targetFriend->removeFromParentAndCleanup(false);
    this->curTargetEntityID = 0;
}

bool InterfaceLayer::withTarget()
{
    return this->curTargetEntityID != 0;
}

void InterfaceLayer::setAction(ActionMode action)
{
    if (action == currentAction)
        return; //same
    
    this->internalTransitionAction(this->getSpriteAction(this->currentAction),
                                   this->getSpriteAction(action));
    this->currentAction = action;
}

void InterfaceLayer::internalTransitionAction(cc::Sprite* prevSprite,
                                              cc::Sprite* nextSprite)
{
    if (prevSprite != nextSprite)
    {
        prevSprite->setOpacity(0);
        nextSprite->setOpacity(255);
    }
}

ActionMode InterfaceLayer::getAction()
{
    return currentAction;
}

ActionMode InterfaceLayer::getNextAction()
{
    return getPrevAction();
}

ActionMode InterfaceLayer::getPrevAction()
{
    if (currentAction == ActionMode::melee)
        return ActionMode::walk;
    return ActionMode::melee;
}

bool InterfaceLayer::init()
{
    if (!Layer::init())
        return false;
    this->targetEnemy = cc::Sprite::createWithSpriteFrameName("target_e.png");
    this->targetEnemy->setAnchorPoint({0,0});
    this->targetEnemy->retain();
    this->targetFriend = cc::Sprite::createWithSpriteFrameName("target_f.png");
    this->targetFriend->setAnchorPoint({0,0});
    this->targetFriend->retain();
    
    this->actionSelection = cc::ui::Scale9Sprite::createWithSpriteFrameName("alt_frame.png");
    actionSelection->setAnchorPoint({0,0});
    actionSelection->setPosition({11, 380});
    actionSelection->setContentSize({180,100});
    this->addChild(actionSelection);
    
    this->actionRun = cc::Sprite::createWithSpriteFrameName("ico_run.png");
    this->actionRun->setAnchorPoint({0,0});
    this->actionRun->setPosition(actionSelection->getPosition() + cc::Point({30.f,12.f}));
    this->currentAction = ActionMode::walk;
    this->addChild(actionRun);
    
    this->actionSword = cc::Sprite::createWithSpriteFrameName("ico_sword.png");
    this->actionSword->setAnchorPoint({0,0});
    this->actionSword->setPosition(actionRun->getPosition());
    this->actionSword->setOpacity(0);
    this->addChild(actionSword);

    this->actionTargetSword = cc::Sprite::createWithSpriteFrameName("ico_target_sword.png");
    this->actionTargetSword->setAnchorPoint({0,0});
    this->actionTargetSword->setPosition(actionRun->getPosition());
    this->actionTargetSword->setOpacity(0);
    this->addChild(actionTargetSword);

    
    auto borders = cc::ui::Scale9Sprite::createWithSpriteFrameName("main_frame.png");
    borders->setAnchorPoint({0,0});
    borders->setPosition(kCanvasRect.origin - cc::Point(10.0f,10.0f));
    borders->setContentSize(kCanvasRect.size + cc::Size(20.0f,20.0f));
    this->addChild(borders);
    
    this->rArrow = cc::Sprite::createWithSpriteFrameName("arrow_right.png");
    this->rArrow->setPosition({155,90});
    
    this->lArrow = cc::Sprite::createWithSpriteFrameName("arrow_right.png");
    this->lArrow->setFlippedX(true);
    this->lArrow->setPosition({25,90});
    
    this->uArrow = cc::Sprite::createWithSpriteFrameName("arrow_up.png");
    this->uArrow->setPosition({94,155});
    
    this->dArrow = cc::Sprite::createWithSpriteFrameName("arrow_up.png");
    this->dArrow->setFlippedY(true);
    this->dArrow->setPosition({94,25});
    
    this->healthBar = HealthBar::create("bar", "health_full", "health_empty");
    this->healthBar->setAnchorPoint({0,0});
    this->healthBar->setPosition({15,600});
    
    this->addChild(this->healthBar);
    
    this->addChild(this->lArrow);
    this->addChild(this->rArrow);
    this->addChild(this->uArrow);
    this->addChild(this->dArrow);
    
    this->cursor = cc::Sprite::createWithSpriteFrameName("joystick2.png");
    this->cursor->setPosition(kCursorCenter);
    this->addChild(this->cursor);

    
    return true;
}

cc::Sprite* InterfaceLayer::getSpriteAction(ActionMode action)
{
    switch(action)
    {
        case ActionMode::walk:
            return this->actionRun;
        case ActionMode::melee:
            if (withTarget())
                return this->actionTargetSword;
            return this->actionSword;
        default:
            CCAssert(false, "not implemented");
            break;
    }
    
    return nullptr;
}