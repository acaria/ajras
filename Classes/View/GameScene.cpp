#include "Headers.h"

bool GameScene::init()
{
    if (!Base::init())
        return false;
    
    auto back = LayerColor::create(Color4B(143,86,59,255));
    this->addChild(back);
    
    this->canvas = ui::Layout::create();
    this->canvas->setLayoutType(LayoutType::RELATIVE);
    this->canvas->setClippingEnabled(true);
    this->canvas->setPosition(kCanvasRect.origin);
    this->canvas->setSize(kCanvasRect.size);
    this->addChild(canvas);
    
    this->frame = Layer::create();
    this->canvas->addChild(this->frame);
    
    this->interface = InterfaceLayer::create();
    this->addChild(interface);
    
    auto spotLight = SpotLight::create(Vec3(-1.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, 0.0f),
                                       Color3B::RED, 0.0, 0.5, 10000.0f) ;
    addChild (spotLight);
    
    return true;
}

void GameScene::setCamera(cocos2d::Vec2 pos)
{
    auto wsize = Director::getInstance()->getWinSize();
    this->frame->setPosition({wsize.width / 2 - pos.x, wsize.height / 2 - pos.y});
}

void GameScene::moveCamera(cocos2d::Vec2 pos, float duration)
{
    auto wsize = Director::getInstance()->getWinSize();
    cocos2d::Vec2 p = {wsize.width / 2 - pos.x, wsize.height / 2 - pos.y};
    
    this->frame->runAction(EaseInOut::create(MoveTo::create(duration, p), 5));
}