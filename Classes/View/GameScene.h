#pragma once

#include "InterfaceLayer.h"
#include "BaseScene.h"
#include "Event.h"
#include "GameCamera.h"

class GameScene : public BaseScene<GameScene>
{
public:
    virtual bool init() override;
    virtual ~GameScene();
    
    lib::Subject<void()> onBack;
    
    //background
    void setBgColor(cc::Color3B bgColor);
    
    InterfaceLayer* interface = nullptr;
    
    //touch event?
    void touchEvent(Ref *pSender, cc::ui::TouchEventType type);
    
    cc::Layer* getFrame();
    GameCamera* getCam();
    
private:
    
    //layers
    cc::ui::Layout* canvas = nullptr;
    cc::Layer* frame = nullptr;
    GameCamera* camera = nullptr;
};
