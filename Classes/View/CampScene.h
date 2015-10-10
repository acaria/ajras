#pragma once

#include "BaseScene.h"
#include "GameCamera.h"

class CampScene : public BaseScene<CampScene>
{
public:
    virtual bool init() override;
    virtual ~CampScene();
    
    enum class CmdTag : int {
        QUIT
    };
    
    lib::Subject<void(CmdTag tag)> onMenuCmd;
    
    //background
    void setBgColor(cc::Color3B bgColor);
    
    cc::Layer* getFrame();
    GameCamera* getCam();
    
private:
    
    //layers
    cc::ui::Layout* canvas = nullptr;
    cc::Layer* frame = nullptr;
    GameCamera* camera = nullptr;
};
