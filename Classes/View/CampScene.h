#pragma once

#include "CampInterface.h"
#include "BaseScene.h"
#include "Event.h"
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
    
    void setBgColor(cc::Color3B bgColor);
    
    CampInterface* interface = nullptr;
    
    cc::Node*   getFrame();
    GameCamera* getCam();
    
private:
    
    //layers
    cc::ui::Layout* canvas = nullptr;
    cc::Node* frame = nullptr;
    GameCamera* camera = nullptr;
};
