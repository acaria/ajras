#include "ControlSystem.h"
#include "Components.h"
#include "InputComponent.h"
#include "RoomData.h"
#include "GameScene.h"
#include "InterfaceLayer.h"

using namespace std::placeholders;
using KeyCode = cocos2d::EventKeyboard::KeyCode;

void ControlSystem::tick(double dt)
{
    for(unsigned index : {INDEX_P1})
    {
        for(auto eid : ecs.join<cp::Input, cp::Control, cp::Render>())
        {
            //direction
            if (ecs::get<cp::Control>(eid) != index)
                continue;
            auto& cpInput = ecs::get<cp::Input>(eid);
         
            if (lib::hasKey(joyDir, index))
            {
                this->view->interface->setJoystick(joyDir[index]);
                cpInput.setDirection(joyDir[index]);
            }
            else
            {
                this->view->interface->clearJoystick();
                cpInput.setDirection(this->curDirPressed[index] |
                    (this->curDirReleased[index] & ~this->preDirPressed[index]));
            }
            
            //selection
            if (this->entitySelection[index] != 0 &&
                ecs::has<cp::Collision>(this->entitySelection[index]))
            {
                auto tid = this->entitySelection[index];
                
                auto& cpRender = ecs::get<cp::Render>(tid);
                auto& cpCol = ecs::get<cp::Collision>(tid);
                
                auto pos = cc::Point(
                    cpCol.rect.getMinX() + cpCol.rect.size.width / 2,
                    cpCol.rect.getMinY() + cpCol.rect.size.height / 2
                );
                
                this->view->interface->setTargetID(tid, ecs::has<cp::Control>(tid), cpRender.container, pos);
                ecs.add<cp::Target>(eid) = tid;
            }
            
            if (this->actionSelection != ActionMode::none)
            {
                cpInput.actionMode = this->actionSelection;
                //data driver, unselect requirements
                if (this->actionSelection == ActionMode::walk)
                {
                    ecs.del<cp::Target>(eid);
                    this->view->interface->clearTarget();
                }
                
                //gui interface
                this->view->interface->setAction(this->actionSelection);
            }
        }
        
        //clear inputs
        this->clearReleased(index);
    }
}

void ControlSystem::initControl(unsigned int index)
{
    curDirPressed[index] = Dir::None;
    curDirReleased[index] = Dir::None;
    preDirPressed[index] = Dir::None;
    entitySelection[index] = 0;
}

void ControlSystem::clearReleased(unsigned index)
{
    this->preDirPressed[index] = this->curDirPressed[index];
    this->curDirReleased[index] = Dir::None;
    this->entitySelection[index] = 0;
    this->actionSelection = ActionMode::none;
}

void ControlSystem::changeRoom(RoomData* data)
{
    this->data = data;
}

void ControlSystem::init(GameScene *gview, RoomData* data)
{
    this->data = data;
    this->view = gview;
    
    this->initControl(INDEX_P1);
    
    //keyboard
    auto kListener = cc::EventListenerKeyboard::create();
    kListener->onKeyPressed = std::bind(&ControlSystem::onKeyPressed, this, _1, _2);
    kListener->onKeyReleased = std::bind(&ControlSystem::onKeyReleased, this, _1, _2);
    
    //mouse
    auto mListener = cc::EventListenerMouse::create();
    mListener->onMouseDown = std::bind(&ControlSystem::onMouseDown, this, _1);
    mListener->onMouseUp = std::bind(&ControlSystem::onMouseUp, this, _1);
    mListener->onMouseMove = std::bind(&ControlSystem::onMouseMove, this, _1);
    mListener->onMouseScroll = std::bind(&ControlSystem::onMouseScroll, this, _1);
    
    //touch
    auto tListener = cc::EventListenerTouchOneByOne::create();
    tListener->onTouchBegan = std::bind(&ControlSystem::onTouchBegan, this, _1, _2);
    tListener->onTouchEnded = std::bind(&ControlSystem::onTouchEnded, this, _1, _2);
    tListener->onTouchMoved = std::bind(&ControlSystem::onTouchMoved, this, _1, _2);
    tListener->onTouchCancelled = std::bind(&ControlSystem::onTouchCanceled, this, _1, _2);
    
    gview->frame->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
        kListener, gview);
    //gview->frame->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
    //    mListener, gview);
    gview->frame->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
        tListener,gview);
}

void ControlSystem::onKeyPressed(KeyCode code, cocos2d::Event *event)
{
    unsigned index = INDEX_P1;
    int toAdd = Dir::None;
    switch(code)
    {
        case KeyCode::KEY_LEFT_ARROW:
        case KeyCode::KEY_A:
            toAdd = Dir::Left;
            break;
        case KeyCode::KEY_RIGHT_ARROW:
        case KeyCode::KEY_D:
            toAdd = Dir::Right;
            break;
        case KeyCode::KEY_UP_ARROW:
        case KeyCode::KEY_W:
            toAdd = Dir::Up;
            break;
        case KeyCode::KEY_DOWN_ARROW:
        case KeyCode::KEY_S:
            toAdd = Dir::Down;
            break;
        case KeyCode::KEY_1:
            actionSelection = ActionMode::walk;
            break;
        case KeyCode::KEY_2:
            actionSelection = ActionMode::melee;
            break;
        default:
            break;
    }
    
    this->curDirPressed[index] |= toAdd;
    this->curDirReleased[index] &= ~toAdd;
}

void ControlSystem::onKeyReleased(KeyCode code, cocos2d::Event *event)
{
    unsigned index = INDEX_P1;
    int toDel = Dir::None;
    switch(code)
    {
        case KeyCode::KEY_LEFT_ARROW:
        case KeyCode::KEY_A:
            toDel = Dir::Left;
            break;
        case KeyCode::KEY_RIGHT_ARROW:
        case KeyCode::KEY_D:
            toDel = Dir::Right;
            break;
        case KeyCode::KEY_UP_ARROW:
        case KeyCode::KEY_W:
            toDel = Dir::Up;
            break;
        case KeyCode::KEY_DOWN_ARROW:
        case KeyCode::KEY_S:
            toDel = Dir::Down;
            break;
        case KeyCode::KEY_1:
            actionSelection = ActionMode::walk;
            break;
        case KeyCode::KEY_2:
            actionSelection = ActionMode::melee;
            break;
        default:
            break;
    }
    
    this->curDirPressed[index] &= ~toDel;
    this->curDirReleased[index] |= toDel;
}

void ControlSystem::onMouseMove(cocos2d::Event *event)
{
    
}

void ControlSystem::onMouseUp(cocos2d::Event *event)
{
}

void ControlSystem::onMouseDown(cocos2d::Event *event)
{
    //unsigned index = INDEX_P1;
    
    cc::Vec2 minColSize = {20,20};
    
    cc::EventMouse* e = (cc::EventMouse*)event;
    
    cc::Vec2 roomPos = {
        data->getBounds().getMinX() + this->view->frame->getPositionX(),
        data->getBounds().getMinY() + this->view->frame->getPositionY()
    };
    
    cc::Vec2 pos = {e->getCursorX() - roomPos.x - kCanvasRect.origin.x,
        e->getCursorY() - roomPos.y - kCanvasRect.origin.y};
    
    Log("mouse pos=%f,%f", pos.x, pos.y);
}

void ControlSystem::onMouseScroll(cocos2d::Event *event)
{
    
}

bool ControlSystem::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event)
{
    unsigned index = INDEX_P1;
    
    cc::Vec2 minColSize = {20,20};
    
    auto touchPos = touch->getLocation();
    
    if (kCanvasRect.containsPoint(touchPos)) // frame zone
    {
        cc::Vec2 roomPos = {
            data->getBounds().getMinX() + this->view->frame->getPositionX(),
            data->getBounds().getMinY() + this->view->frame->getPositionY()
        };
    
        cc::Vec2 pos = {touchPos.x - roomPos.x - kCanvasRect.origin.x,
                touchPos.y - roomPos.y - kCanvasRect.origin.y};

        for(auto eid : ecs.join<cp::Position, cp::Collision, cp::Render>())
        {
            auto& cpPos = ecs::get<cp::Position>(eid);
            auto& cpCol = ecs::get<cp::Collision>(eid);
        
            cc::Vec2 plus = {
                MAX(0, (minColSize.x - cpCol.rect.size.width) / 2),
                MAX(0, (minColSize.y - cpCol.rect.size.height) / 2)
            };
        
            auto bound = cc::Rect(cpPos.pos.x + cpCol.rect.origin.x - plus.x,
                                  cpPos.pos.y + cpCol.rect.origin.y - plus.y,
                                  cpCol.rect.size.width + plus.x * 2,
                                  cpCol.rect.size.height + plus.y * 2);
            if (bound.containsPoint(pos))
            {
                entitySelection[index] = eid;
                break;
            }
        }
    }
    else if (cc::Rect(0,0,190,190).containsPoint(touchPos)) //control zone
    {
        joyID[index] = touch->getID();
        joyDir[index] = (touchPos - kCursorCenter) / 40.f;
    }
    
    return true;
}

void ControlSystem::onTouchCanceled(cocos2d::Touch *touch, cocos2d::Event *event)
{
    
}

void ControlSystem::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    unsigned index = INDEX_P1;
    
    if (lib::hasKey(joyID, index) && joyID[index] == touch->getID())
    {
        joyID.erase(index);
        joyDir.erase(index);
    }
    
    if (this->view->interface->getActionBounds().containsPoint(touch->getStartLocation()))
    {
        auto diff = touch->getLocation() - touch->getStartLocation();
        ActionMode doAction = ActionMode::none;
        if (diff.x < -40)
            doAction = this->view->interface->getPrevAction();
        else if (diff.x > 40)
            doAction = this->view->interface->getNextAction();
        
        if (doAction != ActionMode::none)
        {
            actionSelection = doAction;
        }
    }
}

void ControlSystem::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    unsigned index = INDEX_P1;
    
    if (lib::hasKey(joyID, index) && joyID[index] == touch->getID())
    {
        joyDir[index] = (touch->getLocation() - kCursorCenter) / 40.f;
    }
}