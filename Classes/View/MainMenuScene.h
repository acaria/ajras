#pragma once
#include "BaseScene.h"

class MainMenuScene : public BaseScene<MainMenuScene>
{
public:
    virtual bool init() override;
    
    void goIntro();
    
    enum class ItemTag : int {
        INTRO = 1,
        START = 2,
        QUIT = 3,
    };
        
    lib::Subject<void(ItemTag tag)> onMenuSelect;
        
private:
    void touchEvent(Ref *pSender, cc::ui::Widget::TouchEventType type);
};
