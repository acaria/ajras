#include "DebugSystem.h"
#include "IMapData.h"
#include "LayeredContainer.h"
#include "GameCtrl.h"
#include "Components.h"

DebugSystem::~DebugSystem()
{
    if (this->zoneLayer != nullptr)
        this->zoneLayer->release();
    if (this->drawLayer != nullptr)
        this->drawLayer->release();
}

DebugSystem::DebugSystem() : BaseSystem()
{
    using KeyCode = cocos2d::EventKeyboard::KeyCode;
    
    this->zoneLayer = cc::Layer::create();
    this->zoneLayer->retain();

    this->drawLayer = cc::DrawNode::create();
    this->drawLayer->retain();
}

void DebugSystem::init()
{
    this->eventRegs.push_back(dispatcher->onContextChanged.registerObserver([this](){
        this->zoneLayer->removeFromParent();
        this->zoneLayer->removeAllChildren();
        this->drawLayer->removeFromParent();
        this->drawLayer->removeAllChildren();
        meleeSet.clear();
        meleeMap.clear();
        healthSet.clear();
        healthMap.clear();
        aiSet.clear();
        aiMap.clear();
        gSet.clear();
        gMap.clear();
    }));
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
        if (this->zoneLayer->getParent() != nullptr)
            this->context->view->removeChild(this->zoneLayer);
        if (this->drawLayer->getParent() != nullptr)
            this->context->view->removeChild(this->drawLayer);
        return;
    }
    if (this->zoneLayer->getParent() == nullptr)
        this->context->view->addChild(this->zoneLayer);
    if (this->drawLayer->getParent() == nullptr)
        this->context->view->addChild(this->drawLayer);

    this->displayZones();
    this->displayDraws();
}

cc::Label* DebugSystem::addText(cc::Color3B color, const cc::Rect& bounds, const std::string& txt)
{
    auto label = cc::Label::createWithTTF("", def::font::mini, 8);
    label->getFontAtlas()->setAliasTexParameters();
    label->setPosition({bounds.getMidX(), bounds.getMaxY()});
    this->zoneLayer->addChild(label);
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
    this->zoneLayer->addChild(pixel);
    return pixel;
}

void DebugSystem::displayDraws()
{
    this->drawLayer->clear();
    this->drawLayer->setLineWidth(1);
    
    for(auto& pair : this->context->data->getCol()->agents)
    {
        auto& agent = pair.second;
        this->drawLayer->drawSolidRect(agent.bounds.origin,
                                       agent.bounds.origin + agent.bounds.size,
                                       cc::Color4F(cc::Color4B(0, 255,0,50)));
        this->drawLayer->drawRect(agent.bounds.origin,
                                  agent.bounds.origin + agent.bounds.size,
                                  cc::Color4F(cc::Color4B(0, 255,0,255)));
        this->drawLayer->drawLine({agent.bounds.getMidX(), agent.bounds.getMidY()},
                                  {agent.bounds.getMidX() + agent.velocity.x * 10,
                                   agent.bounds.getMidY() + agent.velocity.y * 10},
                                  cc::Color4F(cc::Color4B(255, 20,117,255)));
    }
}

void DebugSystem::displayZones()
{
    for(auto eid : context->ecs->join<cp::Physics, cp::Position>())
    {
        auto bounds = SysHelper::getBounds(eid);
        
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
        
        auto txt = lib::format("- %u -", eid);
        if (gSet.count(eid) > 0)
        {
            gMap[eid]->setPosition({bounds.getMidX(), bounds.getMaxY() + bounds.size.height + decal});
            gMap[eid]->setString(txt);
        }
        else
        {
            gMap[eid] = addText(cc::Color3B::WHITE, bounds, txt);
            gSet.insert(eid);
        }
        decal += 8;
        
        //health
        if (ecs::has<cp::Health>(eid))
        {
            auto cpHealth = ecs::get<cp::Health>(eid);
            auto txt = lib::format("hp:%.0f/%.0f", cpHealth.hp, cpHealth.maxHp);
            if (ecs::has<cp::Stamina>(eid))
            {
                auto cpStamina = ecs::get<cp::Stamina>(eid);
                txt += lib::format("-end:%.1f", cpStamina.current);
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
    
    this->purge(context->ecs->join<cp::Physics, cp::Position, cp::Melee>(), meleeSet, meleeMap);
    this->purge(context->ecs->join<cp::Physics, cp::Position, cp::Health>(), healthSet, healthMap);
    this->purge(context->ecs->join<cp::Physics, cp::Position, cp::AI>(), aiSet, aiMap);
    this->purge(context->ecs->system<cp::Physics>(), gSet, gMap);
}