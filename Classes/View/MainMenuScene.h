#pragma once
#include "BaseScene.h"

class MainMenuScene : public BaseScene<MainMenuScene>
{
public:
    virtual bool init() override;
    
    void goIntro();
    
    enum class CmdTag : int {
        INTRO,
        START,
        QUIT,
    };
        
    lib::Subject<void(CmdTag tag)> onMenuCmd;
        
private:
    void touchEvent(Ref *pSender, cc::ui::Widget::TouchEventType type);
};
