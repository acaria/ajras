#include "CampInterface.h"
#include "Dir.h"

CampInterface * CampInterface::create()
{
    CampInterface* layer = new (std::nothrow) CampInterface();
    if(layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

CampInterface::CampInterface()
{
}

CampInterface::~CampInterface()
{
}

void CampInterface::registerPlayer(unsigned playerIndex,
                                      std::function<CtrlKeyType(KeyCode)> onKeyCode2KeyType)
{
    this->playerIndex = playerIndex;
    this->onKeyCode2KeyType = onKeyCode2KeyType;
}

StickControl* CampInterface::getStick()
{
    return this->stick;
}

bool CampInterface::init()
{
    if (!Node::init())
        return false;
    
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
            default:
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
        return false;
    };
    
    tListener->onTouchEnded = [this](cc::Touch* touch, cc::Event* event) {
    };
    
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(kListener, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(tListener, this);
    
    return true;
}
