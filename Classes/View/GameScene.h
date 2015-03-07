#pragma once
#include "Headers.h"

using namespace cocos2d;
using namespace ui;

class GameScene : public BaseScene<GameScene>
{
public:
    virtual bool init() override;
    void loadMap(RoomData* data);
    
    event::Subject<void()> onBack;
    void touchEvent(Ref *pSender, ui::TouchEventType type);
};
