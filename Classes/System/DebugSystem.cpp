#include "DebugSystem.h"
#include "IMapData.h"
#include "LayeredContainer.h"
#include "GameCtrl.h"
#include "Components.h"
#include "SysHelper.h"

DebugSystem::DebugSystem(lib::EcsGroup& ecs) : BaseTickSystem(ecs) {
    
}

template<class T>
void DebugSystem::purge(const std::set<unsigned int> &ref,
                        std::set<unsigned int> &destSet,
                        std::map<unsigned int, T> &destMap)
{
    std::set<int> diff;
    std::set_difference(destSet.begin(), destSet.end(), ref.begin(), ref.end(),
                        std::inserter(diff, diff.end()));
    for(auto eid : diff)
    {
        destSet.erase(eid);
        destMap[eid]->removeFromParentAndCleanup(true);
        destMap.erase(eid);
    }
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
        
        //collision
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
        
        //health
        if (ecs::has<cp::Health>(eid))
        {
            auto cpHealth = ecs::get<cp::Health>(eid);
            auto txt = lib::format("hp: %d/%d", cpHealth.hp, cpHealth.maxHp);
            if (healthSet.count(eid) > 0)
            {
                healthMap[eid]->setPosition(bounds.origin);
                healthMap[eid]->setString(txt);
            }
            else
            {
                healthMap[eid] = addText(cc::Color3B::WHITE, bounds, txt);
                healthSet.insert(eid);
            }
        }
    }
    
    this->purge(ecs.join<cp::Collision, cp::Position>(), collisionSet, collisionMap);
    this->purge(ecs.join<cp::Collision, cp::Position, cp::Health>(), healthSet, healthMap);
}

cc::Label* DebugSystem::addText(cc::Color3B color, const cc::Rect& bounds, const std::string& txt)
{
    auto label = cc::Label::createWithTTF("", def::Font::mini, 8);
    label->setPosition({bounds.getMidX(), bounds.getMaxY()});
    this->debugLayer->addChild(label);
    return label;
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