#pragma once

#include "MissionInterface.h"
#include "BaseScene.h"
#include "Event.h"
#include "GameCamera.h"
#include "BufferedFrame.h"

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
    
    MissionInterface* interface = nullptr;
    
    cc::Node* getFrame();
    GameCamera* getCam();
    
private:
    
    //layers
    cc::ui::Layout* canvas = nullptr;
    BufferedFrame* frame = nullptr;
    GameCamera* camera = nullptr;
};
