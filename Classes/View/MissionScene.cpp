#include "MissionScene.h"
#include "MissionInterface.h"
#include "Defines.h"

MissionScene::~MissionScene()
{
    if (this->camera != nullptr)
        delete this->camera;
}

bool MissionScene::init()
{
    if (!Base::init())
        return false;
    
    auto back = cc::LayerColor::create(cc::Color4B(143,86,59,255));
    this->addChild(back);

    this->canvas = cc::ui::Layout::create();
    this->canvas->setBackGroundColorType(cc::ui::HBox::BackGroundColorType::SOLID);
    this->canvas->setBackGroundColor(back->getColor());
    this->canvas->setLayoutType(cc::ui::Layout::Type::RELATIVE);
    this->canvas->setClippingEnabled(true);
    this->canvas->setPosition(def::canvasMissionRect.origin);
    this->canvas->setSize(def::canvasMissionRect.size);
    this->addChild(canvas);
    
    this->frame = cc::Layer::create();
    this->frame->setAnchorPoint({0,0});
    this->canvas->addChild(this->frame);
    
    this->interface = cc::create<MissionInterface>();
    this->addChild(interface);
    
    this->camera = new GameCamera(this->frame, def::canvasMissionRect);
    
    auto listener = cc::EventListenerTouchOneByOne::create();
    //listener->setSwallowTouches(true);
    
    listener->onTouchBegan = [](cc::Touch* touch, cc::Event* event){
        return true;
    };
    
    listener->onTouchEnded = [this](cc::Touch* touch, cc::Event* event){
        if (touch->getLocation().x < 15 && touch->getLocation().y > 625)
        {
            this->onMenuCmd(CmdTag::QUIT);
            return true;
        }
        return false;
    };
    
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void MissionScene::setBgColor(cc::Color3B bgColor)
{
    this->canvas->setBackGroundColor(bgColor);
}

cc::Layer* MissionScene::getFrame()
{
    return this->frame;
}

GameCamera* MissionScene::getCam()
{
    return this->camera;
}