#include "InterfaceLayer.h"
#include "Components.h"
#include "HealthBar.h"
#include "InventoryPanel.h"

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
    //init action positions
    actionExplorePos = {
        {50,    53},
        {37,    53}
    };
    
    actionAttackPos = {
        {92,    53},
        {104,   53}
    };
    
    actionInventorizePos = {
        {137,   53},
        {147,   53}
    };
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

InventoryPanel* InterfaceLayer::getInventoryPanel()
{
    return this->inventoryPanel;
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
    
    if (this->currentAction == ActionMode::attack)
    {
        //auto sprite = this->getSpriteAction(currentAction);
        //if (sprite != actionTargetExplore)
        //    this->internalTransitionAction(sprite, this->actionTargetSword);
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

cc::Vec2 InterfaceLayer::setJoystick(cc::Point pos)
{
    cc::Vec2 result = {
        (pos.x - kCursorCenter.x) / kCursorRegion.x,
        (pos.y - kCursorCenter.y) / kCursorRegion.y};
    
    auto length = result.getLength();
    if (length > 1.0)
        result = result.getNormalized();

    this->cursor->setScaleX((1 - MIN(1.0, length)) * 0.3 + 0.7);
    this->cursor->setRotation(-CC_RADIANS_TO_DEGREES(result.getAngle()));
    
    this->cursor->setPosition(
        kCursorCenter.x + kCursorRegion.x * result.x,
        kCursorCenter.y + kCursorRegion.y * result.y
    );
    
    return result;
}

void InterfaceLayer::clearJoystick()
{
    this->cursor->setRotation(0);
    this->cursor->setScaleX(1.0);
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
    if (this->currentAction == ActionMode::attack)
    {
        //auto sprite = this->getSpriteAction(currentAction);
        //if (sprite == actionTargetSword)
        //    this->internalTransitionAction(sprite, this->actionSword);
    }
    this->targetEnemy->removeFromParentAndCleanup(false);
    this->targetFriend->removeFromParentAndCleanup(false);
    this->curTargetEntityID = 0;
}

bool InterfaceLayer::withTarget()
{
    return this->curTargetEntityID != 0;
}

void InterfaceLayer::setActionPanel(ActionMode action)
{
    if (action == currentAction)
        return; //same

    this->inventoryPanel->stopAllActions();

    switch(action)
    {
        case ActionMode::explore:
            this->inventoryPanel->runAction(cc::FadeOut::create(0.5f));
            break;
        case ActionMode::attack:
            this->inventoryPanel->runAction(cc::FadeOut::create(0.5f));
            break;
        case ActionMode::inventorize:
            this->inventoryPanel->runAction(cc::FadeIn::create(0.5f));
            break;
        case ActionMode::none:
        default:
            Log("invalid action mode");
    }
}

void InterfaceLayer::setActionMode(ActionMode action)
{
    if (action == currentAction)
        return; //same
    
    this->setActionPanel(action);
    
    actionExplore->stopAllActions();
    actionAttack->stopAllActions();
    actionInventorize->stopAllActions();
    actionExploreHi->stopAllActions();
    actionAttackHi->stopAllActions();
    actionInventorizeHi->stopAllActions();
    
    switch(action)
    {
        case ActionMode::explore:
            actionExplore->runAction(cc::Sequence::create(
                cc::Spawn::create(
                    cc::MoveTo::create(0.1, actionSelection->getPosition() + actionExplorePos.first),
                    cc::ScaleTo::create(0.1, 1.0),
                    NULL),
                cc::CallFunc::create([this](){
                    actionExploreHi->runAction(cc::FadeIn::create(0.5));
                }),
                NULL
            ));
            actionAttackHi->runAction(cc::Sequence::create(
                cc::FadeOut::create(0.1),
                cc::CallFunc::create([this](){
                    actionAttack->runAction(cc::Spawn::create(
                        cc::MoveTo::create(0.1, actionSelection->getPosition() + actionAttackPos.second),
                        cc::ScaleTo::create(0.1, 0.7),
                        NULL));
                }),
                NULL
            ));
            actionInventorizeHi->runAction(cc::Sequence::create(
                cc::FadeOut::create(0.1),
                cc::CallFunc::create([this](){
                    actionInventorize->runAction(cc::Spawn::create(
                        cc::MoveTo::create(0.1, actionSelection->getPosition() + actionInventorizePos.second),
                        cc::ScaleTo::create(0.1, 0.7),
                        NULL));
                }),
                NULL
            ));
            break;
        case ActionMode::attack:
            actionExploreHi->runAction(cc::Sequence::create(
                cc::FadeOut::create(0.1),
                cc::CallFunc::create([this](){
                    actionExplore->runAction(cc::Spawn::create(
                        cc::MoveTo::create(0.1, actionSelection->getPosition() + actionExplorePos.second),
                        cc::ScaleTo::create(0.1, 0.7),
                        NULL));
                }),
                NULL
            ));
            actionAttack->runAction(cc::Sequence::create(
                cc::Spawn::create(
                    cc::MoveTo::create(0.1, actionSelection->getPosition() + actionAttackPos.first),
                    cc::ScaleTo::create(0.1, 1.0),
                    NULL),
                cc::CallFunc::create([this](){
                    actionAttackHi->runAction(cc::FadeIn::create(0.5));
                }),
                NULL
            ));
            actionInventorizeHi->runAction(cc::Sequence::create(
                cc::FadeOut::create(0.1),
                cc::CallFunc::create([this](){
                    actionInventorize->runAction(cc::Spawn::create(
                        cc::MoveTo::create(0.1, actionSelection->getPosition() + actionInventorizePos.second),
                        cc::ScaleTo::create(0.1, 0.7),
                        NULL));
                }),
                NULL
            ));
            break;
        case ActionMode::inventorize:
            actionExploreHi->runAction(cc::Sequence::create(
                cc::FadeOut::create(0.1),
                cc::CallFunc::create([this](){
                    actionExplore->runAction(cc::Spawn::create(
                        cc::MoveTo::create(0.1, actionSelection->getPosition() + actionExplorePos.second),
                        cc::ScaleTo::create(0.1, 0.7),
                        NULL));
                }),
                NULL
            ));
            actionAttackHi->runAction(cc::Sequence::create(
                cc::FadeOut::create(0.1),
                cc::CallFunc::create([this](){
                    actionAttack->runAction(cc::Spawn::create(
                        cc::MoveTo::create(0.1, actionSelection->getPosition() +
                                                actionAttackPos.second + cc::Point(-25., 0.)),
                        cc::ScaleTo::create(0.1, 0.7),
                        NULL));
                }),
                NULL
            ));
            actionInventorize->runAction(cc::Sequence::create(
                cc::Spawn::create(
                    cc::MoveTo::create(0.1, actionSelection->getPosition() + actionInventorizePos.first),
                    cc::ScaleTo::create(0.1, 1.0),
                    NULL),
                cc::CallFunc::create([this](){
                    actionInventorizeHi->runAction(cc::FadeIn::create(0.5));
                }),
                NULL
            ));
            break;
        default:
            Log("invalid action mode");
            return;
    }

    this->currentAction = action;
}

ActionMode InterfaceLayer::getAction()
{
    return currentAction;
}

ActionMode InterfaceLayer::getNextAction()
{
    if (currentAction == ActionMode::attack)
        return ActionMode::inventorize;
    if (currentAction == ActionMode::explore)
        return ActionMode::attack;
    return ActionMode::inventorize;
}

ActionMode InterfaceLayer::getPrevAction()
{
    if (currentAction == ActionMode::inventorize)
        return ActionMode::attack;
    if (currentAction == ActionMode::attack)
        return ActionMode::explore;
    return ActionMode::explore;
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
    actionSelection->setPosition({11, 190});
    actionSelection->setContentSize({180,100});
    this->addChild(actionSelection);
    
    this->actionExplore = cc::Sprite::createWithSpriteFrameName("mico_explore.png");
    this->actionExplore->setPosition(actionSelection->getPosition() + actionExplorePos.first);
    this->currentAction = ActionMode::explore;
    this->addChild(actionExplore);
    
    this->actionExploreHi = cc::Sprite::createWithSpriteFrameName("mico_explore_high.png");
    this->actionExploreHi->setPosition(actionSelection->getPosition() + actionExplorePos.first);
    this->addChild(actionExploreHi, actionExplore->getLocalZOrder() - 1);
    
    this->actionAttack = cc::Sprite::createWithSpriteFrameName("mico_attack.png");
    this->actionAttack->setPosition(actionSelection->getPosition() + actionAttackPos.second);
    this->actionAttack->setScale(0.7);
    this->addChild(actionAttack);
    
    this->actionAttackHi = cc::Sprite::createWithSpriteFrameName("mico_attack_high.png");
    this->actionAttackHi->setPosition(actionSelection->getPosition() + actionAttackPos.first);
    this->actionAttackHi->setOpacity(0);
    this->addChild(actionAttackHi, actionAttack->getLocalZOrder() - 1);
    
    this->actionInventorize = cc::Sprite::createWithSpriteFrameName("mico_inventorize.png");
    this->actionInventorize->setPosition(actionSelection->getPosition() + actionInventorizePos.second);
    this->actionInventorize->setScale(0.7);
    this->addChild(actionInventorize);
    
    this->actionInventorizeHi = cc::Sprite::createWithSpriteFrameName("mico_inventorize_high.png");
    this->actionInventorizeHi->setPosition(actionSelection->getPosition() + actionInventorizePos.first);
    this->actionInventorizeHi->setOpacity(0);
    this->addChild(actionInventorizeHi, actionInventorize->getLocalZOrder() - 1);

    //this->actionTargetSword = cc::Sprite::createWithSpriteFrameName("ico_target_sword.png");
    //this->actionTargetSword->setAnchorPoint({0,0});
    //this->actionTargetSword->setPosition(actionRun->getPosition());
    //this->actionTargetSword->setOpacity(0);
    //this->addChild(actionTargetSword);

    
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
    
    this->cursor = cc::Sprite::createWithSpriteFrameName("joystick.png");
    this->cursor->getTexture()->setAntiAliasTexParameters();
    this->cursor->setPosition(kCursorCenter);
    this->addChild(this->cursor);

    this->inventoryPanel = InventoryPanel::create();
    this->inventoryPanel->setAnchorPoint({0,0});
    this->inventoryPanel->setPosition({22.f, 270.f});
    this->inventoryPanel->setOpacity(0);
    this->addChild(inventoryPanel);
    
    return true;
}