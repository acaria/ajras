#pragma once

#include "Defines.h"

class CampInterfaceLayer : public cc::Layer
{
public:
    
    static CampInterfaceLayer* create();
    
    CampInterfaceLayer();
    virtual ~CampInterfaceLayer();
    
    virtual bool init() override;
    
    cc::Vec2    setJoystick(cc::Point pos);
    void        clearJoystick();

private:
    //consts
    const cc::Point kCursorCenter = {90.0f,90.0f};
    const cc::Point kCursorRegion = {30.0f,30.0f};
    
    cc::Sprite* joyStick;
    cc::Sprite* joyBase;

};