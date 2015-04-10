#pragma once
#include "Headers.h"

using namespace cocos2d;
using namespace ui;

class GameScene : public BaseScene<GameScene>
{
public:
    virtual bool init() override;
    
    event::Subject<void()> onBack;
    
    //layers
    cc::ui::Layout* canvas = nullptr;
    cocos2d::Layer* frame = nullptr;
    
    //background
    void setBgColor(cc::Color3B bgColor);
    
    InterfaceLayer* interface = nullptr;
    
    //touch event?
    void touchEvent(Ref *pSender, ui::TouchEventType type);
    
    //frame camera
    void setCamera(cocos2d::Vec2 pos);
    void moveCamera(cocos2d::Vec2 pos, float duration);
};
