#include "GameScene.h"
#include "InterfaceLayer.h"
#include "Defines.h"

GameScene::~GameScene()
{
    if (this->camera != nullptr)
        delete this->camera;
}

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
    this->frame->setAnchorPoint({0,0});
    this->canvas->addChild(this->frame);
    
    this->interface = InterfaceLayer::create();
    this->addChild(interface);
    
    this->camera = new GameCamera(this->frame, kCanvasRect);
    
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

GameCamera* GameScene::getCam()
{
    return this->camera;
}