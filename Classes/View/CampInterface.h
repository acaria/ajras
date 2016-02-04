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

    //accessors
    StickControl*   getStick();

    //events
    lib::Subject<void(KeyCode)> onKeyPressAction;
    lib::Subject<void(KeyCode)> onKeyReleaseAction;

private:
    
    StickControl* stick;
};