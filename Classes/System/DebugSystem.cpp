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
    if (GameCtrl::data()->debugMode == 0)
    {
        if (this->zoneLayer->getParent() != nullptr)
            this->context->view->removeChild(this->zoneLayer);
        if (this->drawLayer->getParent() != nullptr)
            this->context->view->removeChild(this->drawLayer);
        return;
    }
    if (this->zoneLayer->getParent() == nullptr)
        this->context->view->addChild(this->zoneLayer, 100);
    if (this->drawLayer->getParent() == nullptr)
        this->context->view->addChild(this->drawLayer, 100);

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
        unsigned eid = pair.first;
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
        
        cc::Point agentPivot = agent.bounds.origin + agent.bounds.size / 2;
        
        //display sight range
        /*if (ecs::has<cp::AI>(pair.first))
        {
            auto& cpAI = ecs::get<cp::AI>(pair.first);
            this->drawLayer->drawCircle(agentPivot,
                                        cpAI.sightRange.first,
                                        CC_DEGREES_TO_RADIANS(90), 30, false,
                                        cc::Color4F(cc::Color4B(0, 255, 0, 255)));
            this->drawLayer->drawCircle(agentPivot,
                                        cpAI.sightRange.second,
                                        CC_DEGREES_TO_RADIANS(90), 30, false,
                                        cc::Color4F(cc::Color4B(255, 0, 255, 255)));
        }*/
        
        if (ecs::has<cp::Debug>(eid) && ecs::get<cp::Debug>(eid).wayPoints.size() > 0)
        {
            cc::Point origin = agentPivot;
            for(auto destPoint : ecs::get<cp::Debug>(eid).wayPoints)
            {
                this->drawLayer->drawLine(origin, destPoint, cc::Color4F::WHITE);
                this->drawLayer->drawSolidCircle(destPoint, 2, 0, 10, cc::Color4F::WHITE);
                origin = destPoint;
            }
        }
        
        if (ecs::has<cp::Melee>(eid))
        {
            
            auto& cpMelee = ecs::get<cp::Melee>(eid);
            this->drawLayer->drawSolidRect(
                cpMelee.atkRect.origin,
                {cpMelee.atkRect.getMaxX(), cpMelee.atkRect.getMaxY()},
                cc::Color4F(0,0,1,0.2));
        }
        
        if (ecs::has<cp::Trail>(eid))
        {
            auto& cpTrail = ecs::get<cp::Trail>(eid);
            
            auto coords = cpTrail.tail;
            if (coords.size() > 1)
            {
                auto it = coords.begin();
                for(int i = 0; i < (coords.size() - 1); i++)
                {
                    auto el = this->context->data->getBlockBound(*it);
                    auto el2 = this->context->data->getBlockBound(*(++it));
                    this->drawLayer->drawLine({el.getMidX(), el.getMidY()},
                        {el2.getMidX(), el2.getMidY()}, cc::Color4F(0,0,1,1));
                }
            }
        }
    }
}

void DebugSystem::displayZones()
{
    for(auto eid : context->ecs->join<cp::Physics, cp::Position>())
    {
        auto bounds = SysHelper::getBounds(eid);
        
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
    
    this->purge(context->ecs->join<cp::Physics, cp::Position, cp::Health>(), healthSet, healthMap);
    this->purge(context->ecs->join<cp::Physics, cp::Position, cp::AI>(), aiSet, aiMap);
    this->purge(context->ecs->system<cp::Physics>(), gSet, gMap);
}