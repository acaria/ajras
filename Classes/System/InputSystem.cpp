#include "Headers.h"

using namespace std::placeholders;
using KeyCode = cocos2d::EventKeyboard::KeyCode;

void InputSystem::tick(double dt)
{
    unsigned orientation = this->curDirPressed |
    (this->curDirReleased & ~this->preDirPressed);
    
    for(auto eid : ecs.join<cp::Input, cp::Orientation>())
    {
        auto& cpOrientation = ecs::get<cp::Orientation>(eid);
        
        if ((orientation & this->savedOrientation) == 0)
            ecs::get<cp::Velocity>(eid).accelFactor = 0.0f;
        
        if (orientation != Dir::kNone)
        {
            ecs::get<cp::Orientation>(eid).lastDir = ecs::get<cp::Orientation>(eid).curDir;
            
            if (cpOrientation.lastDir != Dir::kNone && ((cpOrientation.lastDir & orientation) == cpOrientation.lastDir))
                cpOrientation.curDir = cpOrientation.lastDir;
            else
                ecs::get<cp::Orientation>(eid).curDir = orientation;
        }
        
        if (ecs::has<cp::Velocity>(eid))
        {
            ecs::get<cp::Velocity>(eid).direction = lib::getVectorDirection(orientation);
        }
    }
    
    this->savedOrientation = orientation;
    
    this->clearReleased();
}

void InputSystem::animate(double dt, double tickPercent)
{
}

void InputSystem::clearReleased()
{
    this->preDirPressed = this->curDirPressed;
    this->curDirReleased = Dir::kNone;
}

void InputSystem::init(cocos2d::Node *view)
{
    curDirPressed = Dir::kNone;
    curDirReleased = Dir::kNone;
    preDirPressed = Dir::kNone;
    
    auto kListener = EventListenerKeyboard::create();
    
    kListener->onKeyPressed = std::bind(&InputSystem::onKeyPressed, this, _1, _2);
    kListener->onKeyReleased = std::bind(&InputSystem::onKeyReleased, this, _1, _2);
    
    view->getEventDispatcher()->addEventListenerWithSceneGraphPriority(kListener, view);
}

void InputSystem::onKeyPressed(KeyCode code, cocos2d::Event *event)
{
    int toAdd = Dir::kNone;
    switch(code)
    {
        case KeyCode::KEY_LEFT_ARROW:
        case KeyCode::KEY_A:
            toAdd = Dir::kLeft;
            break;
        case KeyCode::KEY_RIGHT_ARROW:
        case KeyCode::KEY_D:
            toAdd = Dir::kRight;
            break;
        case KeyCode::KEY_UP_ARROW:
        case KeyCode::KEY_W:
            toAdd = Dir::kUp;
            break;
        case KeyCode::KEY_DOWN_ARROW:
        case KeyCode::KEY_S:
            toAdd = Dir::kDown;
            break;
        default:
            break;
    }
    
    this->curDirPressed |= toAdd;
    this->curDirReleased &= ~toAdd;
}

void InputSystem::onKeyReleased(KeyCode code, cocos2d::Event *event)
{
    int toDel = Dir::kNone;
    switch(code)
    {
        case KeyCode::KEY_LEFT_ARROW:
        case KeyCode::KEY_A:
            toDel = Dir::kLeft;
            break;
        case KeyCode::KEY_RIGHT_ARROW:
        case KeyCode::KEY_D:
            toDel = Dir::kRight;
            break;
        case KeyCode::KEY_UP_ARROW:
        case KeyCode::KEY_W:
            toDel = Dir::kUp;
            break;
        case KeyCode::KEY_DOWN_ARROW:
        case KeyCode::KEY_S:
            toDel = Dir::kDown;
            break;
        default:
            break;
    }
    
    this->curDirPressed &= ~toDel;
    this->curDirReleased |= toDel;
}
