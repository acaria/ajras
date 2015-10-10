#pragma once

#include "MissionInterfaceLayer.h"
#include "BaseScene.h"
#include "Event.h"
#include "GameCamera.h"

class MissionScene : public BaseScene<MissionScene>
{
public:
    virtual bool init() override;
    virtual ~MissionScene();
    
    enum class CmdTag : int {
        QUIT
    };
    
    lib::Subject<void(CmdTag tag)> onMenuCmd;
    
    //background
    void setBgColor(cc::Color3B bgColor);
    
    MissionInterfaceLayer* interface = nullptr;
    
    cc::Layer* getFrame();
    GameCamera* getCam();
    
private:
    
    //layers
    cc::ui::Layout* canvas = nullptr;
    cc::Layer* frame = nullptr;
    GameCamera* camera = nullptr;
};
