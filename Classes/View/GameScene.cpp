#include "Headers.h"

bool GameScene::init()
{
    if (!Base::init())
        return false;
    
    auto back = LayerColor::create(Color4B::WHITE);
    this->addChild(back);
    
    this->canvas = Layer::create();
    this->addChild(canvas);
    
    this->frame = Layer::create();
    this->canvas->addChild(this->frame);
    
    this->interface = InterfaceLayer::create();
    this->addChild(interface);
    
    return true;
}

void GameScene::setCamera(cocos2d::Vec2 pos)
{
    auto wsize = Director::getInstance()->getWinSize();
    this->frame->setPosition(wsize.width / 2 - pos.x, wsize.height / 2 - pos.y);
}

void GameScene::moveCamera(cocos2d::Vec2 pos, float duration)
{
    auto wsize = Director::getInstance()->getWinSize();
    cocos2d::Vec2 p = {wsize.width / 2 - pos.x, wsize.height / 2 - pos.y};
    
    this->frame->runAction(EaseInOut::create(MoveTo::create(duration, p), 5));
}