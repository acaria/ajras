#pragma once
#include "BaseScene.h"

class MainMenuScene : public BaseScene<MainMenuScene>
{
public:
    virtual bool init() override;
    
    enum class ItemTag : int {
        START = 1,
        OPTIONS = 2,
        CREDITS = 3,
        QUIT = 4,
    };
        
    lib::Subject<void(ItemTag tag)> onMenuSelect;
        
private:
        void touchEvent(Ref *pSender, cc::ui::Widget::TouchEventType type);
};
