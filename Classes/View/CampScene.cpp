#include "CampScene.h"
#include "Defines.h"

CampScene::~CampScene()
{
    if (this->camera != nullptr)
        delete this->camera;
}

bool CampScene::init()
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
    this->canvas->setPosition(def::canvasCampRect.origin);
    this->canvas->setSize(def::canvasCampRect.size);
    this->addChild(canvas);
    
    this->frame = cc::Layer::create();
    this->frame->setAnchorPoint({0,0});
    this->canvas->addChild(this->frame);
    
    this->interface = CampInterface::create();
    this->addChild(interface);

    this->camera = new GameCamera(this->frame, def::canvasCampRect);
    
    auto listener = cc::EventListenerTouchOneByOne::create();
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

void CampScene::setBgColor(cc::Color3B bgColor)
{
    this->canvas->setBackGroundColor(bgColor);
}

cc::Layer* CampScene::getFrame()
{
    return this->frame;
}

GameCamera* CampScene::getCam()
{
    return this->camera;
}