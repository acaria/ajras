#include "TestScene.h"
#include "SpriteEx.h"
#include "SpriteLib.h"

bool TestScene::init()
{
    auto s = new SpriteLight();
    s->initWithSpriteFrameName("pixel.png");
    s->setColor(cc::Color3B::BLUE);
    s->setPosition(100,100);
    s->setScale(200, 200);

    auto s2 = new SpriteLight();
    s2->initWithSpriteFrameName("logo.png");
    s2->setPosition(500,500);
    
    this->addChild(s);
    this->addChild(s2);
    
    s->release();

    

    return true;
}
