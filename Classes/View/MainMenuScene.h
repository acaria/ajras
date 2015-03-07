#pragma once
#include "Headers.h"

using namespace cocos2d;
using namespace ui;

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
        
    event::Subject<void(ItemTag tag)> onMenuSelect;
        
private:
    void touchEvent(Ref *pSender, Widget::TouchEventType type);
};
