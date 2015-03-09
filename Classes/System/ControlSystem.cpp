#include "Headers.h"

using namespace std::placeholders;
using KeyCode = cocos2d::EventKeyboard::KeyCode;

void ControlSystem::tick(double dt)
{
    unsigned orientation = this->curDirPressed |
    (this->curDirReleased & ~this->preDirPressed);
    
    for(auto eid : ecs.join<cp::Input, cp::Control>())
    {
        auto& cpInput = ecs::get<cp::Input>(eid);
        
        cpInput.lastOrientation = cpInput.orientation;
        cpInput.orientation = orientation;
    }
    
    this->savedOrientation = orientation;
    
    this->clearReleased();
}

void ControlSystem::clearReleased()
{
    this->preDirPressed = this->curDirPressed;
    this->curDirReleased = Dir::kNone;
}

void ControlSystem::init(cocos2d::Node *view)
{
    curDirPressed = Dir::kNone;
    curDirReleased = Dir::kNone;
    preDirPressed = Dir::kNone;
    
    auto kListener = EventListenerKeyboard::create();
    
    kListener->onKeyPressed = std::bind(&ControlSystem::onKeyPressed, this, _1, _2);
    kListener->onKeyReleased = std::bind(&ControlSystem::onKeyReleased, this, _1, _2);
    
    view->getEventDispatcher()->addEventListenerWithSceneGraphPriority(kListener, view);
}

void ControlSystem::onKeyPressed(KeyCode code, cocos2d::Event *event)
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

void ControlSystem::onKeyReleased(KeyCode code, cocos2d::Event *event)
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
