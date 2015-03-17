#include "Headers.h"

InterfaceLayer * InterfaceLayer::create()
{
    InterfaceLayer * layer = new (std::nothrow) InterfaceLayer();
    if(layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}


InterfaceLayer::InterfaceLayer()
{
}

InterfaceLayer::~InterfaceLayer()
{
    this->targetEnemy->release();
    this->targetFriend->release();
    Layer::~Layer();
}

void InterfaceLayer::setTargetID(unsigned eid, bool friendly)
{
    if (eid == this->curTargetEntityID)
        return;
    if (!ecs::has<cp::Render, cp::Collision>(eid))
    {
        Log("impossible to target without required components");
        return;
    }
    
    this->clearTarget();
    
    auto& cpRender = ecs::get<cp::Render>(eid);
    auto& cpCol = ecs::get<cp::Collision>(eid);
    
    Sprite* target = friendly ? this->targetFriend : this->targetEnemy;

    cpRender.container->addChild(target, -1);
    target->setPosition({
        cpCol.rect.getMinX() + cpCol.rect.size.width / 2 - targetEnemy->getTextureRect().size.width / 2,
        cpCol.rect.getMinY() + cpCol.rect.size.height / 2 - targetEnemy->getTextureRect().size.height / 2
    });
    
    this->curTargetEntityID = eid;
}

void InterfaceLayer::clearTarget()
{
    this->targetEnemy->removeFromParentAndCleanup(false);
    this->targetFriend->removeFromParentAndCleanup(false);
    this->curTargetEntityID = 0;
}

bool InterfaceLayer::init()
{
    if (!Layer::init())
        return false;
    this->targetEnemy = Sprite::createWithSpriteFrameName("target_e.png");
    this->targetEnemy->setAnchorPoint({0,0});
    this->targetEnemy->retain();
    this->targetFriend = Sprite::createWithSpriteFrameName("target_f.png");
    this->targetFriend->setAnchorPoint({0,0});
    this->targetFriend->retain();
    
    return true;
}