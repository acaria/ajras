#include "GameScene.h"
#include "InterfaceLayer.h"

bool GameScene::init()
{
    if (!Base::init())
        return false;
    
    auto back = cc::LayerColor::create(cc::Color4B(143,86,59,255));
    this->addChild(back);
    
    this->canvas = cc::ui::Layout::create();
    this->canvas->setBackGroundColorType(cc::ui::HBox::BackGroundColorType::SOLID);
    this->canvas->setBackGroundColor(back->getColor());
    this->canvas->setLayoutType(cc::ui::LayoutType::RELATIVE);
    this->canvas->setClippingEnabled(true);
    this->canvas->setPosition(kCanvasRect.origin);
    this->canvas->setSize(kCanvasRect.size);
    this->addChild(canvas);
    
    this->frame = cc::Layer::create();
    this->canvas->addChild(this->frame);
    
    this->interface = InterfaceLayer::create();
    this->addChild(interface);
    
    return true;
}

void GameScene::setBgColor(cc::Color3B bgColor)
{
    this->canvas->setBackGroundColor(bgColor);
}

void GameScene::setCamera(cocos2d::Vec2 pos)
{
    auto wsize = kCanvasRect.size;
    this->frame->setPosition({wsize.width / 2 - pos.x, wsize.height / 2 - pos.y});
}

void GameScene::moveCamera(cocos2d::Vec2 pos, float duration)
{
    auto wsize = kCanvasRect.size;
    cocos2d::Vec2 p = {wsize.width / 2 - pos.x, wsize.height / 2 - pos.y};
    
    this->frame->runAction(cc::EaseInOut::create(cc::MoveTo::create(duration, p), 5));
}