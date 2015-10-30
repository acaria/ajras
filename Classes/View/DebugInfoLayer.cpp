#include "DebugInfoLayer.h"
#include "Defines.h"
#include "GameCtrl.h"

bool DebugInfoLayer::init()
{
    auto wSize = cc::Director::getInstance()->getWinSize();
    auto panel = cc::LayerColor::create(cc::Color4B::BLACK, wSize.width, 18);
    panel->setAnchorPoint({0,0});
    panel->setOpacity(100);
    panel->setPosition(0, wSize.height - 18);
    this->addChild(panel);
    
    this->info = cc::Label::createWithTTF("", def::font::normal, 16);
    this->info->setAnchorPoint({0,0});

    panel->addChild(this->info);
    
    auto kListener = cc::EventListenerKeyboard::create();
    kListener->onKeyReleased = [this](KeyCode code, cocos2d::Event *event) {
        auto& tick = GameCtrl::instance()->tick;
        switch(code)
        {
            case KeyCode::KEY_P:
                if (tick.getTimeScale() == 0)
                    tick.setTimeScale(1.0);
                else
                    tick.setTimeScale(0);
                break;
            case KeyCode::KEY_EQUAL:
                tick.setTimeScale(tick.getTimeScale() * 1.2);
                break;
            case KeyCode::KEY_MINUS:
                tick.setTimeScale(tick.getTimeScale() / 1.2);
                break;
            default: //nothing
                break;
        }
    };
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(kListener, this);

    return true;
}

void DebugInfoLayer::update()
{
    auto& tick = GameCtrl::instance()->tick;
    
    auto txt = lib::format(" fps:%-2d calls:%-4lu verts:%-6lu tick:%-6lu scale:%1.2f",
        tick.getFPS(),
        cc::Director::getInstance()->getRenderer()->getDrawnBatches(),
        cc::Director::getInstance()->getRenderer()->getDrawnVertices(),
        tick.getTicks(),
        tick.getTimeScale());
    this->info->setString(txt);
}