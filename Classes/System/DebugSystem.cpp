#include "DebugSystem.h"
#include "IMapData.h"
#include "LayeredContainer.h"
#include "GameCtrl.h"
#include "Components.h"
#include "SysHelper.h"

DebugSystem::DebugSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {
    
}

void DebugSystem::tick(double dt)
{
    if (GameCtrl::instance()->getData().debugMode == 0)
    {
        if (this->debugLayer->getParent() != nullptr)
            this->view->removeChild(this->debugLayer);
        return;
    }
    if (this->debugLayer->getParent() == nullptr)
        this->view->addChild(this->debugLayer);

    for(auto eid : ecs.join<cp::Collision, cp::Position>())
    {
        auto bounds = SysHelper::getBounds(eid);
        if (collisionSet.count(eid) > 0)
        {
            collisionMap[eid]->setPosition(bounds.origin);
            collisionMap[eid]->setScale(bounds.size.width, bounds.size.height);
        }
        else
        {
            collisionMap[eid] = addPixel(cc::Color3B::GREEN, bounds);
            collisionSet.insert(eid);
        }
    }
    
    std::set<int> diff;
    auto collisionRef = ecs.join<cp::Collision, cp::Position>();
    std::set_difference(collisionSet.begin(), collisionSet.end(), collisionRef.begin(), collisionRef.end(),
                        std::inserter(diff, diff.end()));
    
    for(auto eid : diff)
    {
        collisionSet.erase(eid);
        collisionMap[eid]->removeFromParentAndCleanup(true);
        collisionMap.erase(eid);
    }
}

cc::Sprite* DebugSystem::addPixel(cc::Color3B color, const cc::Rect& bounds)
{
    auto pixel = cc::Sprite::createWithSpriteFrameName("pixel.png");
    pixel->setOpacity(100);
    pixel->setColor(color);
    pixel->setAnchorPoint({0,0});
    pixel->setPosition(bounds.origin);
    pixel->setScale(bounds.size.width, bounds.size.height);
    this->debugLayer->addChild(pixel);
    return pixel;
}

DebugSystem::~DebugSystem()
{
    if (this->debugLayer != nullptr)
        this->debugLayer->release();
}

void DebugSystem::init(LayeredContainer *view, IMapData *data)
{
    using KeyCode = cocos2d::EventKeyboard::KeyCode;
    
    this->view = view;
    this->data = data;
    
    this->debugLayer = cc::Layer::create();
    this->debugLayer->retain();
}