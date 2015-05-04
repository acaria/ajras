#pragma once

#include "InterfaceLayer.h"
#include "BaseScene.h"
#include "Event.h"

class GameScene : public BaseScene<GameScene>
{
public:
    virtual bool init() override;
    
    lib::Subject<void()> onBack;
    
    //layers
    cc::ui::Layout* canvas = nullptr;
    cc::Layer* frame = nullptr;
    
    //background
    void setBgColor(cc::Color3B bgColor);
    
    InterfaceLayer* interface = nullptr;
    
    //touch event?
    void touchEvent(Ref *pSender, cc::ui::TouchEventType type);
    
    //frame camera
    void setCamera(cc::Vec2 pos);
    void moveCamera(cc::Vec2 pos, float duration);
};
