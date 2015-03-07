#include "Headers.h"

bool GameScene::init()
{
    if (!Base::init())
        return false;
    
    auto back = LayerColor::create(Color4B::WHITE);
    
    this->addChild(back);
    
    return true;
}