#include "DebugSystem.h"
#include "IMapData.h"
#include "LayeredContainer.h"
#include "GameCtrl.h"
#include "Components.h"
#include "SysHelper.h"

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
            this->context->view->removeChild(this->debugLayer);
        return;
    }
    if (this->debugLayer->getParent() == nullptr)
        this->context->view->addChild(this->debugLayer);

    for(auto eid : context->ecs->join<cp::Physics, cp::Position>())
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
        
        //melee
        if (ecs::has<cp::Melee>(eid))
        {
            auto& cpMelee = ecs::get<cp::Melee>(eid);
            if (meleeSet.count(eid) > 0)
            {
                meleeMap[eid]->setPosition(cpMelee.atkRect.origin);
                meleeMap[eid]->setScale(cpMelee.atkRect.size.width, cpMelee.atkRect.size.height);
            }
            else
            {
                meleeMap[eid] = addPixel(cc::Color3B::YELLOW, bounds);
                meleeSet.insert(eid);
            }
        }
        
        unsigned decal = 0;
        
        //health
        if (ecs::has<cp::Health>(eid))
        {
            auto cpHealth = ecs::get<cp::Health>(eid);
            auto txt = lib::format("hp: %.0f/%.0f", cpHealth.hp, cpHealth.maxHp);
            if (ecs::has<cp::Stamina>(eid))
            {
                auto cpStamina = ecs::get<cp::Stamina>(eid);
                txt += lib::format(" - end:%.1f", cpStamina.current);
            }
            if (healthSet.count(eid) > 0)
            {
                healthMap[eid]->setPosition({bounds.getMidX(), bounds.getMaxY() + bounds.size.height + decal});
                healthMap[eid]->setString(txt);
            }
            else
            {
                healthMap[eid] = addText(cc::Color3B::WHITE, bounds, txt);
                healthSet.insert(eid);
            }
            decal += 8;
        }
        
        //AI
        if (ecs::has<cp::AI>(eid))
        {
            auto cpAI = ecs::get<cp::AI>(eid);
            if (aiSet.count(eid) > 0)
            {
                aiMap[eid]->setPosition({bounds.getMidX(), bounds.getMaxY() + bounds.size.height + decal});
                aiMap[eid]->setString(cpAI.board.lastAction);
            }
            else
            {
                aiMap[eid] = addText(cc::Color3B::WHITE, bounds, cpAI.board.lastAction);
                aiSet.insert(eid);
            }
            decal += 8;
        }
    }
    
    this->purge(context->ecs->join<cp::Physics, cp::Position>(), collisionSet, collisionMap);
    this->purge(context->ecs->join<cp::Physics, cp::Position, cp::Melee>(), meleeSet, meleeMap);
    this->purge(context->ecs->join<cp::Physics, cp::Position, cp::Health>(), healthSet, healthMap);
    this->purge(context->ecs->join<cp::Physics, cp::Position, cp::AI>(), aiSet, aiMap);
}

cc::Label* DebugSystem::addText(cc::Color3B color, const cc::Rect& bounds, const std::string& txt)
{
    auto label = cc::Label::createWithTTF("", def::font::mini, 8);
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

DebugSystem::DebugSystem() : BaseSystem()
{
    using KeyCode = cocos2d::EventKeyboard::KeyCode;
    this->debugLayer = cc::Layer::create();
    this->debugLayer->retain();
}

void DebugSystem::init()
{
    this->eventRegs.push_back(dispatcher->onContextChanged.registerObserver([this](){
        this->debugLayer->removeFromParent();
        this->debugLayer->removeAllChildren();
        collisionSet.clear();
        collisionMap.clear();
        meleeSet.clear();
        meleeMap.clear();
        healthSet.clear();
        healthMap.clear();
        aiSet.clear();
        aiMap.clear();
    }));
}