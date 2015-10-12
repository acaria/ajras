#include "MissionInterface.h"
#include "Dir.h"

using KeyCode = cocos2d::EventKeyboard::KeyCode;

MissionInterface * MissionInterface::create()
{
    MissionInterface * layer = new (std::nothrow) MissionInterface();
    if(layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

MissionInterface::MissionInterface()
{
    //init action positions
    actionTeamPos = {
        {50,    53},
        {37,    53}
    };
    
    actionInventorizePos = {
        {92,    53},
        {104,   53}
    };
    
    actionMapPos = {
        {137,   53},
        {147,   53}
    };
}

MissionInterface::~MissionInterface()
{
    this->targetEnemy->release();
    this->targetFriend->release();
}

void MissionInterface::registerPlayer(unsigned playerIndex,
    std::function<CtrlKeyType(KeyCode)> onKeyCode2KeyType)
{
    this->playerIndex = playerIndex;
    this->onKeyCode2KeyType = onKeyCode2KeyType;
}

HealthBar* MissionInterface::getHealthBar()
{
    return this->healthBar;
}

StickControl* MissionInterface::getStick()
{
    return this->stick;
}

InventoryPanel* MissionInterface::getInventoryPanel()
{
    return this->inventoryPanel;
}

void MissionInterface::setTargetID(unsigned eid, bool friendly, cc::Sprite* container, cc::Point pos)
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
    
    this->curTargetEntityID = eid;
}

void MissionInterface::unsetTargetID(unsigned int eid)
{
    if (eid == this->curTargetEntityID)
    {
        this->clearTarget();
    }
}

cc::Rect MissionInterface::getActionBounds()
{
    return cc::Rect(this->actionSelection->getPosition().x,
                    this->actionSelection->getPosition().y,
                    this->actionSelection->getContentSize().width,
                    this->actionSelection->getContentSize().height);
}

void MissionInterface::clearTarget()
{
    this->targetEnemy->removeFromParentAndCleanup(false);
    this->targetFriend->removeFromParentAndCleanup(false);
    this->curTargetEntityID = 0;
}

bool MissionInterface::withTarget()
{
    return this->curTargetEntityID != 0;
}

void MissionInterface::setActionPanel(ActionMode action)
{
    if (action == currentAction)
        return; //same

    this->inventoryPanel->stopAllActions();

    switch(action)
    {
        case ActionMode::team:
            this->inventoryPanel->runAction(cc::FadeOut::create(0.5f));
            break;
        case ActionMode::map:
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

void MissionInterface::setActionMode(ActionMode action)
{
    if (action == currentAction)
        return; //same
    
    this->setActionPanel(action);
    
    actionTeam->stopAllActions();
    actionMap->stopAllActions();
    actionInventorize->stopAllActions();
    actionTeamHi->stopAllActions();
    actionMapHi->stopAllActions();
    actionInventorizeHi->stopAllActions();
    
    switch(action)
    {
        case ActionMode::team:
            actionTeam->runAction(cc::Sequence::create(
                cc::Spawn::create(
                    cc::MoveTo::create(0.1, actionSelection->getPosition() + actionTeamPos.first),
                    cc::ScaleTo::create(0.1, 1.0),
                    NULL),
                cc::CallFunc::create([this](){
                    actionTeamHi->runAction(cc::FadeIn::create(0.5));
                }),
                NULL
            ));
            actionMapHi->runAction(cc::Sequence::create(
                cc::FadeOut::create(0.1),
                cc::CallFunc::create([this](){
                    actionMap->runAction(cc::Spawn::create(
                        cc::MoveTo::create(0.1, actionSelection->getPosition() + actionMapPos.second),
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
        case ActionMode::map:
            actionTeamHi->runAction(cc::Sequence::create(
                cc::FadeOut::create(0.1),
                cc::CallFunc::create([this](){
                    actionTeam->runAction(cc::Spawn::create(
                        cc::MoveTo::create(0.1, actionSelection->getPosition() + actionTeamPos.second),
                        cc::ScaleTo::create(0.1, 0.7),
                        NULL));
                }),
                NULL
            ));
            actionMap->runAction(cc::Sequence::create(
                cc::Spawn::create(
                    cc::MoveTo::create(0.1, actionSelection->getPosition() + actionMapPos.first),
                    cc::ScaleTo::create(0.1, 1.0),
                    NULL),
                cc::CallFunc::create([this](){
                    actionMapHi->runAction(cc::FadeIn::create(0.5));
                }),
                NULL
            ));
            actionInventorizeHi->runAction(cc::Sequence::create(
                cc::FadeOut::create(0.1),
                cc::CallFunc::create([this](){
                    actionInventorize->runAction(cc::Spawn::create(
                        cc::MoveTo::create(0.1, actionSelection->getPosition() + actionInventorizePos.second + cc::Point(-25., 0.)),
                        cc::ScaleTo::create(0.1, 0.7),
                        NULL));
                }),
                NULL
            ));
            break;
        case ActionMode::inventorize:
            actionTeamHi->runAction(cc::Sequence::create(
                cc::FadeOut::create(0.1),
                cc::CallFunc::create([this](){
                    actionTeam->runAction(cc::Spawn::create(
                        cc::MoveTo::create(0.1, actionSelection->getPosition() + actionTeamPos.second),
                        cc::ScaleTo::create(0.1, 0.7),
                        NULL));
                }),
                NULL
            ));
            actionMapHi->runAction(cc::Sequence::create(
                cc::FadeOut::create(0.1),
                cc::CallFunc::create([this](){
                    actionMap->runAction(cc::Spawn::create(
                        cc::MoveTo::create(0.1,
                            actionSelection->getPosition() +
                            actionMapPos.second),
                        cc::ScaleTo::create(0.1, 0.7),
                        NULL));
                }),
                NULL
            ));
            actionInventorize->runAction(cc::Sequence::create(
                cc::Spawn::create(
                    cc::MoveTo::create(0.1,
                            actionSelection->getPosition() +
                            actionInventorizePos.first),
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

    this->onSetActionMode(action);
    this->currentAction = action;
}

ActionMode MissionInterface::getAction()
{
    return currentAction;
}

ActionMode MissionInterface::getNextAction()
{
    if (currentAction == ActionMode::map)
        return ActionMode::team;
    if (currentAction == ActionMode::team)
        return ActionMode::inventorize;
    return ActionMode::map;
}

ActionMode MissionInterface::getPrevAction()
{
    if (currentAction == ActionMode::inventorize)
        return ActionMode::team;
    if (currentAction == ActionMode::map)
        return ActionMode::inventorize;
    return ActionMode::map;
}

bool MissionInterface::init()
{
    if (!Node::init())
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
    
    this->actionTeam = cc::Sprite::createWithSpriteFrameName("mico_team.png");
    this->actionTeam->setPosition(actionSelection->getPosition() + actionTeamPos.first);
    this->currentAction = ActionMode::team;
    this->addChild(actionTeam);
    
    this->actionTeamHi = cc::Sprite::createWithSpriteFrameName("mico_team_high.png");
    this->actionTeamHi->setPosition(actionSelection->getPosition() + actionTeamPos.first);
    this->addChild(actionTeamHi, actionTeam->getLocalZOrder() - 1);
    
    this->actionMap = cc::Sprite::createWithSpriteFrameName("mico_map.png");
    this->actionMap->setPosition(actionSelection->getPosition() + actionMapPos.second);
    this->actionMap->setScale(0.7);
    this->addChild(actionMap);
    
    this->actionMapHi = cc::Sprite::createWithSpriteFrameName("mico_map_high.png");
    this->actionMapHi->setPosition(actionSelection->getPosition() + actionMapPos.first);
    this->actionMapHi->setOpacity(0);
    this->addChild(actionMapHi, actionMap->getLocalZOrder() - 1);
    
    this->actionInventorize = cc::Sprite::createWithSpriteFrameName("mico_inventorize.png");
    this->actionInventorize->setPosition(actionSelection->getPosition() + actionInventorizePos.second);
    this->actionInventorize->setScale(0.7);
    this->addChild(actionInventorize);
    
    this->actionInventorizeHi = cc::Sprite::createWithSpriteFrameName("mico_inventorize_high.png");
    this->actionInventorizeHi->setPosition(actionSelection->getPosition() + actionInventorizePos.first);
    this->actionInventorizeHi->setOpacity(0);
    this->addChild(actionInventorizeHi, actionInventorize->getLocalZOrder() - 1);


    auto borders = cc::ui::Scale9Sprite::createWithSpriteFrameName("main_frame.png");
    borders->setAnchorPoint({0,0});
    borders->setPosition(def::canvasMissionRect.origin - cc::Point(10.0f,10.0f));
    borders->setContentSize(def::canvasMissionRect.size + cc::Size(20.0f,20.0f));
    this->addChild(borders);
    
    this->healthBar = HealthBar::create("bar", "health_full", "health_empty");
    this->healthBar->setAnchorPoint({0,0});
    this->healthBar->setPosition({15,600});
    
    this->addChild(this->healthBar);

    this->inventoryPanel = InventoryPanel::create();
    this->inventoryPanel->setAnchorPoint({0,0});
    this->inventoryPanel->setPosition({22.f, 270.f});
    this->inventoryPanel->setOpacity(0);
    this->addChild(inventoryPanel);
    
    this->stick = StickControl::create("joy2.png", "joy1.png", 90, 30);
    this->stick->setPosition(90, 90);
    this->addChild(this->stick);
    
    //keyboard
    auto kListener = cc::EventListenerKeyboard::create();
    kListener->onKeyPressed = [this](KeyCode code, cocos2d::Event *event) {
        assert(this->onKeyCode2KeyType);
        int toAdd = Dir::None;
        
        switch(this->onKeyCode2KeyType(code))
        {
            case CtrlKeyType::none:
                break;
            case CtrlKeyType::left:
                toAdd = Dir::Left;
                break;
            case CtrlKeyType::right:
                toAdd = Dir::Right;
                break;
            case CtrlKeyType::up:
                toAdd = Dir::Up;
                break;
            case CtrlKeyType::down:
                toAdd = Dir::Down;
                break;
            case CtrlKeyType::autoselect:
                //todo
                /*
                 if (player->entityFocus != 0)
                 {
                 auto tid = SysHelper::getNearest(ecs.getID(),
                 player->entityFocus,
                 AIComponent::eMood::HOSTILE,
                 500.0);
                 if (tid != 0)
                 {
                 entitySelection[player->ctrlIndex] = tid;
                 }
                 }
                 */
                break;
            case CtrlKeyType::sel1:
                this->setActionMode(ActionMode::team);
                break;
            case CtrlKeyType::sel2:
                this->setActionMode(ActionMode::inventorize);
                break;
            case CtrlKeyType::sel3:
                this->setActionMode(ActionMode::map);
                break;
        }
        
        this->onKeyPressAction(this->playerIndex, toAdd);
    };
    
    kListener->onKeyReleased = [this](KeyCode code, cocos2d::Event *event) {
        assert(this->onKeyCode2KeyType);
        int toDel = Dir::None;
        
        switch(this->onKeyCode2KeyType(code))
        {
            default:
                break;
            case CtrlKeyType::left: toDel = Dir::Left;
                break;
            case CtrlKeyType::right: toDel = Dir::Right;
                break;
            case CtrlKeyType::up: toDel = Dir::Up;
                break;
            case CtrlKeyType::down: toDel = Dir::Down;
                break;
                
        }
        
        this->onKeyReleaseAction(this->playerIndex, toDel);
    };
    
    auto tListener = cc::EventListenerTouchOneByOne::create();
    tListener->setSwallowTouches(false);
    
    tListener->onTouchBegan = [this](cc::Touch* touch, cc::Event* event) {
        if (this->getActionBounds().containsPoint(touch->getLocation()))
            return true;
        return false;
    };
    
    tListener->onTouchEnded = [this](cc::Touch* touch, cc::Event* event) {
        auto diff = touch->getLocation() - touch->getStartLocation();

        if (diff.x < - 40)
            this->setActionMode(this->getPrevAction());
        else if (diff.x > 40)
            this->setActionMode(this->getNextAction());
    };
    
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(kListener, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(tListener, this);
    
    return true;
}

