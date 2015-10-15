#include "RoomSystemCtrl.h"
#include "RoomData.h"
#include "LayeredNode.h"
#include "Components.h"
#include "ModelProvider.h"
#include "CsActionInterval.h"
#include "MissionScene.h"
#include "HealthBar.h"
#include "GateMap.h"
#include "Defines.h"

RoomSystemCtrl::RoomSystemCtrl(): renderSystem(ecsGroup),
        collisionSystem(ecsGroup),
        moveSystem(ecsGroup),
        transSystem(ecsGroup),
        inputSystem(ecsGroup),
        meleeSystem(ecsGroup),
        targetSystem(ecsGroup),
        aiSystem(ecsGroup),
#if ECSYSTEM_DEBUG
        debugSystem(ecsGroup),
#endif
        interactSystem(ecsGroup) {
    this->registerEvents();
}

void RoomSystemCtrl::tick(double dt)
{
    aiSystem.tick(dt);
    inputSystem.tick(dt);
    targetSystem.tick(dt);
    moveSystem.tick(dt);
    collisionSystem.tick(dt);
    meleeSystem.tick(dt);
    transSystem.tick(dt);
    renderSystem.tick(dt);
    interactSystem.tick(dt);
#if ECSYSTEM_DEBUG
    debugSystem.tick(dt);
#endif
}

void RoomSystemCtrl::animate(double dt, double tickPercent)
{
    aiSystem.animate(dt, tickPercent);
    inputSystem.animate(dt, tickPercent);
    targetSystem.animate(dt, tickPercent);
    moveSystem.animate(dt, tickPercent);
    collisionSystem.animate(dt, tickPercent);
    meleeSystem.animate(dt, tickPercent);
    transSystem.animate(dt, tickPercent);
    renderSystem.animate(dt, tickPercent);
    interactSystem.animate(dt, tickPercent);
#if ECSYSTEM_DEBUG
    debugSystem.animate(dt, tickPercent);
#endif
}

void RoomSystemCtrl::loadRoom(LayeredNode *view, RoomData *data)
{
    ecsGroup.setID(data->index);
    
    auto roomIndex = data->index;
    auto grid = data->getContent();
    
    //todo: try not generating entities for bg maps
    
    //map
    for(unsigned j = 0; j < grid.height; j++)
        for(unsigned i = 0; i < grid.width; i++)
        {
            auto properties = grid.get(i, j).fields;
            if (properties.find(BlockInfo::bgTileName) != properties.end())
            {
                //HACK: put most of tiles in bg layer
                auto rl = view->bg;
                if (properties.find(BlockInfo::collision) != properties.end() &&
                    properties[BlockInfo::collision] == "flyable")
                {
                    rl = view->main;
                }
                
                auto coord = data->getPosFromCoord({i,j});
                auto sprite = cc::Sprite::createWithSpriteFrameName(
                        properties[BlockInfo::bgTileName]);
                sprite->getTexture()->setAntiAliasTexParameters();
                sprite->setAnchorPoint({0, 0});
                sprite->setPosition(coord);
                rl->addChild(sprite, data->getZOrder(coord));
            }
            
            if (properties.find(BlockInfo::fgTileName) != properties.end())
            {
                auto coord = data->getPosFromCoord({i,j});
                auto sprite = cc::Sprite::createWithSpriteFrameName(
                        properties[BlockInfo::fgTileName]);
                sprite->getTexture()->setAntiAliasTexParameters();
                sprite->setAnchorPoint({0, 0});
                sprite->setPosition(coord);
                view->fg->addChild(sprite, data->getZOrder(coord));
            }
        }
    
    //objects
    for(auto obj : data->getModelObjs())
    {
        if (!lib::hasKey(obj.properties, "profile"))
            continue;
        auto profile = ModelProvider::instance()->profile.get(obj.properties["profile"]);
        auto eid = cp::entity::genID();
        ecs::add<cp::Render>(eid, roomIndex).setProfile(profile,
            RenderComponent::chooseLayer(profile, view),
            data->getZOrder(obj.pos));
        ecs::get<cp::Render>(eid).sprite->setPosition(obj.pos - ecs::get<cp::Collision>(eid).rect.origin);
        ecs::add<cp::Collision>(eid, roomIndex).setProfile(profile);
        ecs::add<cp::Input>(eid, roomIndex);
        ecs::add<cp::Position>(eid, roomIndex).set(obj.pos - ecs::get<cp::Collision>(eid).rect.origin);
        
        if (profile->withMove)
        {
            if (profile->orientation)
                ecs::add<cp::Orientation>(eid, roomIndex);
            ecs::add<cp::Velocity>(eid, roomIndex).setProfile(profile);
        }
        
        if (profile->withMelee)
        {
            ecs::add<cp::Melee>(eid, roomIndex).setProfile(profile);
        }
        
        if (profile->withBehaviour)
        {
            ecs::add<cp::AI>(eid, roomIndex).setProfile(profile);
        }
        
        if (profile->withHealth)
        {
            ecs::add<cp::Health>(eid, roomIndex).setProfile(profile);
        }
        
        if (profile->withInteraction)
        {
            ecs::add<cp::Interact>(eid, roomIndex).setProfile(profile);
        }
        
        if (obj.properties["profile"] == "torch")
        {
            ecs::get<cp::Render>(eid).setAnimation("activated", -1);
            
            auto light = cc::Sprite::createWithSpriteFrameName("grad_ellipse.png");
            //light->setOpacity(120);
            light->setColor(cc::Color3B(252, 195, 159));
            light->setBlendFunc(cc::BlendFunc::ADDITIVE);
            light->setPosition(obj.pos + cc::Vec2(8, 8));
            light->runAction(cc::RepeatForever::create(Flicker::create(
                80.0f, 0.1f, {150, 200}, {0.98, 1.2}, {0.9,1.1},
                cc::Color3B(252, 168, 50), cc::Color3B(252, 168, 50))));
            view->fg->addChild(light, 1);
        }
    }
    
    //gates
    for(auto element : data->gateMapping)
    {
        GateMap gateMap = element.second;
        auto eid = cp::entity::genID();
        ecs::add<cp::Gate>(eid, roomIndex) = gateMap;
        
        //view
        cc::Point pos = {gateMap.info.rect.origin.x, gateMap.info.rect.origin.y};
        auto sprite = cc::Sprite::createWithSpriteFrameName(gateMap.tileName + ".png");
        sprite->getTexture()->setAntiAliasTexParameters();
        sprite->setAnchorPoint({0, 0});
        sprite->setPosition(pos);
        view->bg->addChild(sprite, data->getZOrder(pos));
        
        //change collision data
        
        auto gateSrcCoord = data->getCoordFromPos(pos);
        auto gateDestCoord = data->getCoordFromPos({
            gateMap.info.rect.getMaxX(),
            gateMap.info.rect.getMaxY()});
        
        for(int j = gateSrcCoord.y; j < gateDestCoord.y; j++)
        for(int i = gateSrcCoord.x; i < gateDestCoord.x; i++)
        {
            data->getContent().get(i, j).fields[BlockInfo::collision] = "walkable";
        }
    }
    
    //init systems
    collisionSystem.init(data);
    renderSystem.init(data);
    aiSystem.init(data);
#if ECSYSTEM_DEBUG
    debugSystem.init(view, data);
#endif
}

void RoomSystemCtrl::hideObjects(float duration)
{
    for(auto eid : ecsGroup.join<cp::Render, cp::AI>())
    {
        if (duration == 0)
            ecs::get<cp::Render>(eid).sprite->setOpacity(0);
        else
        {
            ecs::get<cp::Render>(eid).sprite->runAction(cc::FadeOut::create(duration));
        }
    }
}

void RoomSystemCtrl::showObjects(float duration)
{
    for(auto eid : ecsGroup.join<cp::Render, cp::AI>())
    {
        if (duration == 0)
            ecs::get<cp::Render>(eid).sprite->setOpacity(255);
        else
        {
            ecs::get<cp::Render>(eid).sprite->runAction(cc::FadeIn::create(duration));
        }
    }
}

void RoomSystemCtrl::registerEvents()
{
    this->eventRegs.push_back(meleeSystem.onHealthChanged.registerObserver([this](unsigned eid, int health){
        this->onHealthChanged(this->ecsGroup.getID(), eid, health);
    }));
    this->eventRegs.push_back(transSystem.onGateTriggered.registerObserver([this](unsigned eid, GateMap gate){
        this->onGateTriggered(this->ecsGroup.getID(), eid, gate);
    }));
}