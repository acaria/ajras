#include "Headers.h"

void RoomSystemCtrl::tick(double dt)
{
    aiSystem.tick(dt);
    inputSystem.tick(dt);
    targetSystem.tick(dt);
    moveSystem.tick(dt);
    collisionSystem.tick(dt);
    meleeSystem.tick(dt);
    gateSystem.tick(dt);
    renderSystem.tick(dt);
}

void RoomSystemCtrl::animate(double dt, double tickPercent)
{
    aiSystem.animate(dt, tickPercent);
    inputSystem.animate(dt, tickPercent);
    targetSystem.animate(dt, tickPercent);
    moveSystem.animate(dt, tickPercent);
    collisionSystem.animate(dt, tickPercent);
    meleeSystem.animate(dt, tickPercent);
    gateSystem.animate(dt, tickPercent);
    renderSystem.animate(dt, tickPercent);
}

void RoomSystemCtrl::load(RoomLayer *view, RoomData *data)
{
    collisionSystem.init(data);
    renderSystem.init(data);
    ecsGroup.setID(data->index);
    
    //88888888888888888
    /*auto sLayer = Layer::create();
     sLayer->setPosition({10,10});
     auto shape = this->currentMap->shape;
     for(unsigned j = 0; j < shape.size.y; j++)
     for(unsigned i = 0; i < shape.size.x; i++)
     {
     auto pxl = Sprite::createWithSpriteFrameName("pixel.png");
     pxl->getTexture()->setAliasTexParameters();
     pxl->setPosition((int)(i * 5),(int)( j * 5));
     pxl->setScale(5.0f, 5.0f);
     sLayer->addChild(pxl);
     
     switch(shape.get(i,j))
     {
     case MapShape::NONE:
     pxl->setColor(Color3B::GRAY);
     break;
     case MapShape::WALL:
     pxl->setColor(Color3B::RED);
     break;
     case MapShape::GATE:
     pxl->setColor(Color3B::MAGENTA);
     break;
     }
     }
     view->addChild(sLayer);*/
    //88888888888888888
    
    auto roomIndex = data->index;
    auto grid = data->getModel()->grid;
        
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
                
            auto coord = data->getModel()->getPosCoord({i,j});
            auto sprite = Sprite::createWithSpriteFrameName(properties[BlockInfo::bgTileName]);
            sprite->getTexture()->setAliasTexParameters();
            sprite->setAnchorPoint({0, 0});
            sprite->setPosition(coord);
            rl->addChild(sprite, data->getModel()->getZOrder(coord));
        }
    }
        
    //objects
    for(auto obj : data->getModel()->objs)
    {
        auto eid = cp::entity::genID();
        ecs::add<cp::Render>(eid, roomIndex).setProfile(obj.profileName,
            view->main, data->getModel()->getZOrder(obj.pos));
        ecs::add<cp::Collision>(eid, roomIndex).setProfile(obj.profileName);
        ecs::add<cp::Cat>(eid, roomIndex).setProfile(obj.profileName);
        ecs::add<cp::Position>(eid, roomIndex).set(obj.pos - ecs::get<cp::Collision>(eid).rect.origin);
            
        if (obj.profileName == "boy") //player
        {
            ecs::add<cp::Orientation>(eid, roomIndex);
            ecs::add<cp::Velocity>(eid, roomIndex).set(80.0, 0.3, 0.2);
            ecs::add<cp::Input>(eid, roomIndex);
            ecs::add<cp::Control>(eid, roomIndex) = ControlSystem::INDEX_P1;
            ecs::add<cp::Melee>(eid, roomIndex).set("atk", MeleeComponent::DIR, 12);
            ecs::add<cp::Health>(eid, roomIndex).set(20);
        }
            
        if (obj.profileName == "zomb")
        {
            ecs::add<cp::Orientation>(eid, roomIndex);
            ecs::add<cp::Velocity>(eid, roomIndex).set(80.0, 0.3, 0.2);
            ecs::add<cp::Input>(eid, roomIndex);
            ecs::add<cp::Health>(eid, roomIndex).set(3);
            ecs::add<cp::AI>(eid, roomIndex).setProfile(obj.profileName);
            ecs::add<cp::Melee>(eid, roomIndex).set("atk", MeleeComponent::DIR, 12);
        }
            
        if (obj.profileName == "torch")
        {
            ecs::get<cp::Render>(eid).setAnimation("activated", -1);
                
            auto light = Sprite::createWithSpriteFrameName("grad_ellipse.png");
            //light->setOpacity(120);
            light->setColor(Color3B(252, 195, 159));
            light->setBlendFunc(BlendFunc::ADDITIVE);
            light->setPosition(obj.pos + Vec2(8, 8));
            light->runAction(RepeatForever::create(Flicker::create(80.0f, 0.1f,
                {150, 200}, {0.98, 1.2}, {0.9,1.1}, Color3B(252, 168, 50), Color3B(252, 168, 50))));
            view->fg->addChild(light, 1);
        }
    }
        
    for(auto gateMap : data->gateMapping)
    {
        unsigned srcGateIndex = gateMap.first;
        unsigned destRoomIndex = gateMap.second.first;
        unsigned destGateIndex = gateMap.second.second;
        
        auto eid = cp::entity::genID();
        ecs::add<cp::Gate>(eid, roomIndex).set(
            destRoomIndex, destGateIndex, data->getModel()->gates[srcGateIndex]);
    }
}