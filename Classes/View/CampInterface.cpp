#include "CampInterface.h"
#include "Dir.h"

CampInterface::~CampInterface()
{
}

void CampInterface::registerIndex(unsigned index, const KeyCode2TypeFunc& onKeyCode2KeyType)
{
    this->controlMap[index] = onKeyCode2KeyType;
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
        for(auto el : this->controlMap)
        {
            assert(el.second);
            int toAdd = Dir::None;
        
            switch(el.second(code))
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
            this->onKeyPressAction(el.first, toAdd);
        }
    };
    
    kListener->onKeyReleased = [this](KeyCode code, cocos2d::Event *event) {
        for(auto el : this->controlMap)
        {
            assert(el.second);
            int toDel = Dir::None;
        
            switch(el.second(code))
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
        
            this->onKeyReleaseAction(el.first, toDel);
        }
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
