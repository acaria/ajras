#pragma once

#include "Defines.h"
#include "StickControl.h"
#include "Event.h"

using KeyCode = cocos2d::EventKeyboard::KeyCode;

class CampInterface : public cc::Node
{
public:
    using KeyCode2TypeFunc = std::function<CtrlKeyType(KeyCode)>;

    virtual ~CampInterface();
    
    virtual bool init() override;
    
    void registerIndex(unsigned playerIndex, const KeyCode2TypeFunc& onKeyCode2KeyType);

    //accessors
    StickControl*   getStick();

    //events
    lib::Subject<void(unsigned, int)> onKeyPressAction;
    lib::Subject<void(unsigned, int)> onKeyReleaseAction;

private:
    std::map<unsigned, KeyCode2TypeFunc> controlMap;
    
    StickControl* stick;
};