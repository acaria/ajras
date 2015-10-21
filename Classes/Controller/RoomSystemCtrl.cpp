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

RoomSystemCtrl::RoomSystemCtrl(): random(Randgine::instance()->get(Randgine::FLOOR)),
        renderSystem(ecsGroup),
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
    this->forwardEvents();
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
                sprite->setAnchorPoint({0, 0});
                sprite->setPosition(coord);
                rl->addChild(sprite, data->getZOrder(coord));
            }
            
            if (properties.find(BlockInfo::fgTileName) != properties.end())
            {
                auto coord = data->getPosFromCoord({i,j});
                auto sprite = cc::Sprite::createWithSpriteFrameName(
                        properties[BlockInfo::fgTileName]);
                sprite->setAnchorPoint({0, 0});
                sprite->setPosition(coord);
                view->fg->addChild(sprite, data->getZOrder(coord));
            }
        }
    
    //objects
    for(auto obj : data->getModelObjs())
    {
        if (lib::hasKey(obj.properties, "profile"))
            this->loadStaticObject(obj.properties["profile"], obj.pos, data, view);
        else if (lib::hasKey(obj.properties, "zone_type"))
        {
            this->loadZoneObject(obj.properties["zone_type"],
                {obj.pos.x, obj.pos.y, obj.size.width, obj.size.height}, data, view);
        }
        else
        {
            Log("invalid obj detected: name:%s, type:%s", obj.name.c_str(), obj.type.c_str());
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
    //hack: gerate collision data after twicks on gates
    data->getCol()->process();
    
    //init systems
    collisionSystem.init(data->getCol());
    interactSystem.init(data->getCol());
    renderSystem.init(data);
    aiSystem.init(data);
#if ECSYSTEM_DEBUG
    debugSystem.init(view, data);
#endif
}

void RoomSystemCtrl::loadZoneObject(const std::string &zoneType, const cc::Rect &bounds, RoomData *data, LayeredNode *view)
{
    auto roomIndex = data->index;
    std::string profileName;
    
    bool process = true;
    switch(lib::hash(zoneType))
    {
        case lib::hash("foe"): {
            profileName = "bat";
            break;
        }
        case lib::hash("reward"): {
            profileName = "chest1";
            break;
        }
        default:
        {
            process = false;
            Log("invalid zone type: %s", zoneType.c_str());
        }
    }
    
    if (process)
    {
        auto profile = ModelProvider::instance()->profile.get(profileName);
        cc::Point pos = {
            bounds.origin.x + this->random.interval(0, bounds.size.width - profile->collisionRect.getMaxX()),
            bounds.origin.y + this->random.interval(0, bounds.size.height - profile->collisionRect.getMaxY())
        };
        this->loadStaticObject(profileName, pos, data, view);
    }
}

void RoomSystemCtrl::loadStaticObject(const std::string &profileName,
                                      const cc::Point& pos,
                                      RoomData *data,
                                      LayeredNode *view)
{
    auto roomIndex = data->index;
    auto profile = ModelProvider::instance()->profile.get(profileName);
    auto eid = cp::entity::genID();
    ecs::add<cp::Render>(eid, roomIndex).setProfile(profile,
                                                    RenderComponent::chooseLayer(profile, view),
                                                    data->getZOrder(pos));
    ecs::get<cp::Render>(eid).sprite->setPosition(pos - ecs::get<cp::Collision>(eid).rect.origin);
    ecs::add<cp::Collision>(eid, roomIndex).setProfile(profile);
    ecs::add<cp::Input>(eid, roomIndex);
    ecs::add<cp::Position>(eid, roomIndex).set(pos - ecs::get<cp::Collision>(eid).rect.origin);
    
    if (profile->stats != nullptr)
    {
        auto stats = profile->stats.Value;
        if (stats.move != nullptr)
        {
            auto move = stats.move.Value;
            if (move.orientation)
                ecs::add<cp::Orientation>(eid, roomIndex);
            ecs::add<cp::Velocity>(eid, roomIndex).setProfile(profile);
        }
        
        if (stats.melee != nullptr)
        {
            ecs::add<cp::Melee>(eid, roomIndex).setProfile(profile);
        }
        
        if (stats.health != nullptr)
        {
            ecs::add<cp::Health>(eid, roomIndex).setProfile(profile);
        }
    }
    
    if (profile->withBehaviour)
    {
        ecs::add<cp::AI>(eid, roomIndex).setProfile(profile);
    }
    
    if (profile->interaction != nullptr)
    {
        ecs::add<cp::Interact>(eid, roomIndex).setProfile(profile);
        
        if (profile->interaction.Value.actionType == ProfileInteractInfo::ActionType::REWARD)
        {
            assert(profile->interaction.Value.actionParams != nullptr);
            auto collectables = ModelProvider::instance()->collectible.genReward(
                                                                                 random, profile->interaction.Value.actionParams.Value);
            cp::GearComponent reward;
            for(auto collectable : collectables)
            {
                reward.push_back(SlotData {
                    .quantity = 1,
                    .content = collectable
                });
            }
            ecs::add<cp::Gear>(eid, roomIndex) = reward;
        }
    }
    
    if (profileName == "torch")
    {
        ecs::get<cp::Render>(eid).setAnimation("activated", -1);
        
        auto light = cc::Sprite::createWithSpriteFrameName("grad_ellipse.png");
        //light->setOpacity(120);
        light->setColor(cc::Color3B(252, 195, 159));
        light->setBlendFunc(cc::BlendFunc::ADDITIVE);
        light->setPosition(pos + cc::Vec2(8, 8));
        light->runAction(cc::RepeatForever::create(Flicker::create(
                                                                   80.0f, 0.1f, {150, 200}, {0.98, 1.2}, {0.9,1.1},
                                                                   cc::Color3B(252, 168, 50), cc::Color3B(252, 168, 50))));
        view->fg->addChild(light, 1);
    }
}

CollisionSystem& RoomSystemCtrl::getCollisionSystem()
{
    return collisionSystem;
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

void RoomSystemCtrl::forwardEvents()
{
    this->eventRegs.push_back(meleeSystem.onHealthChanged.registerObserver([this](unsigned eid, int health){
        this->onHealthChanged(this->ecsGroup.getID(), eid, health);
    }));
    this->eventRegs.push_back(transSystem.onGateTriggered.registerObserver([this](unsigned eid, GateMap gate){
        this->onGateTriggered(this->ecsGroup.getID(), eid, gate);
    }));
}