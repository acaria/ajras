#include "GameScene.h"
#include "InterfaceLayer.h"
#include "Defines.h"

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
    this->canvas->setPosition(patchPos(kCanvasRect.origin));
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

cc::Layer* GameScene::getFrame()
{
    return this->frame;
}

void GameScene::setCamera(cocos2d::Vec2 pos)
{
    auto wsize = kCanvasRect.size;
    this->frame->setPosition(cc::Point(
        floor(wsize.width / 2 - pos.x) + 0.04,
        floor(wsize.height / 2 - pos.y) + 0.04));
}

void GameScene::translateCamera(cc::Vec2 translation)
{
    auto pos = this->frame->getPosition() + translation;
    this->frame->setPosition(cc::Point(
        floor(pos.x) + 0.04,
        floor(pos.y) + 0.04));
}

cc::Point GameScene::getCameraOrigin() const
{
    return this->frame->getPosition();
}

void GameScene::moveCamera(cocos2d::Vec2 pos, float duration)
{
    auto wsize = kCanvasRect.size;
    cc::Vec2 p = cc::Point(
        floor(wsize.width / 2 - pos.x) + 0.04,
        floor(wsize.height / 2 - pos.y) + 0.04);
    
    this->frame->runAction(cc::EaseInOut::create(cc::MoveTo::create(duration, p), 5));
}