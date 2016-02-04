#include "CampInterface.h"
#include "Dir.h"

CampInterface::~CampInterface()
{
}

StickControl* CampInterface::getStick()
{
    return this->stick;
}

bool CampInterface::init()
{
    if (!Node::init())
        return false;
    
    this->stick = cc::create<StickControl>("joy2.png", "joy1.png", 90, 30);
    this->stick->setPosition(90, 90);
    this->addChild(this->stick);
    
    //keyboard
    auto kListener = cc::EventListenerKeyboard::create();
    kListener->onKeyPressed = [this](KeyCode code, cocos2d::Event *event) {
        this->onKeyPressAction(code);
    };
    
    kListener->onKeyReleased = [this](KeyCode code, cocos2d::Event *event) {
        this->onKeyReleaseAction(code);
    };
    
    auto tListener = cc::EventListenerTouchOneByOne::create();
    tListener->setSwallowTouches(false);
    
    tListener->onTouchBegan = [this](cc::Touch* touch, cc::Event* event) {
        return false;
    };
    
    tListener->onTouchEnded = [this](cc::Touch* touch, cc::Event* event) {
    };
    
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(kListener, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(tListener, this);
    
    return true;
}
