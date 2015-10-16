#pragma once

#include "Defines.h"
#include "StickControl.h"
#include "Event.h"

using KeyCode = cocos2d::EventKeyboard::KeyCode;

class CampInterface : public cc::Node
{
public:
    virtual ~CampInterface();
    
    virtual bool init() override;
    
    void registerPlayer(unsigned playerIndex,
                        std::function<CtrlKeyType(KeyCode)> onKeyCode2KeyType);

    //accessors
    StickControl*   getStick();

    //events
    lib::Subject<void(unsigned, int)> onKeyPressAction;
    lib::Subject<void(unsigned, int)> onKeyReleaseAction;

private:
    std::function<CtrlKeyType(KeyCode)> onKeyCode2KeyType = nullptr;
    
    StickControl* stick;
    
    unsigned playerIndex = 0;
};