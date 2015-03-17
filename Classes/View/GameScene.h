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
    cocos2d::Layer* canvas = nullptr;
    cocos2d::Layer* frame = nullptr;
    InterfaceLayer* interface = nullptr;
    
    //touch event?
    void touchEvent(Ref *pSender, ui::TouchEventType type);
    
    //frame camera
    void setCamera(cocos2d::Vec2 pos);
    void moveCamera(cocos2d::Vec2 pos, float duration);
};
