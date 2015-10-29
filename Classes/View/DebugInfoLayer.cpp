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

    return true;
}

void DebugInfoLayer::update()
{
    auto tick = GameCtrl::instance()->tick;
    auto renderer = cc::Director::getInstance()->getRenderer();
    
    auto txt = lib::format(" fps:%-2d calls:%-4lu verts:%-6lu tick:%-6lu scale:%1.2f",
        tick.getFPS(), renderer->getDrawnBatches(), renderer->getDrawnVertices(), tick.getTicks(), tick.getTimeScale());
    this->info->setString(txt);
}