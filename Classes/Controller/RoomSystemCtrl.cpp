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

void RoomSystemCtrl::loadStart(RoomLayer *view, RoomData *data)
{
    this->loadRoom(view, data);
 
    auto roomIndex = data->index;
    assert(data->getModel()->warps.size() > 0);
    
    auto warpInfo = data->getModel()->warps.front();
    auto srcPos = warpInfo.getSrcPos();
    auto destPos = warpInfo.getDestPos();
    
    //create player
    auto pName = "boy";
    auto eid = cp::entity::genID();
    
    auto& cpRender = ecs::add<cp::Render>(eid, roomIndex);
    auto& cpCollision = ecs::add<cp::Collision>(eid, roomIndex);
    
    cpRender.setProfile(pName, view->main, data->getModel()->getZOrder(srcPos));
    cpCollision.setProfile(pName);
    
    ecs::add<cp::Cat>(eid, roomIndex).setProfile(pName);
    cpRender.container->setPosition({
        srcPos.x - cpCollision.rect.getMinX() - cpCollision.rect.size.width / 2,
        srcPos.y - cpCollision.rect.getMinY() - cpCollision.rect.size.height / 2
    });
    
    float duration = 3.0f;
    
    cpRender.container->setCascadeOpacityEnabled(true);
    cpRender.container->setOpacity(0);
    cpRender.container->runAction(Sequence::create(
        MoveTo::create(duration, {
            destPos.x - cpCollision.rect.getMinX() - cpCollision.rect.size.width / 2,
            destPos.y - cpCollision.rect.getMinY() - cpCollision.rect.size.height / 2
        }),
        CallFunc::create([eid, roomIndex, cpRender](){
            ecs::add<cp::Position>(eid, roomIndex).set(cpRender.container->getPosition());
            ecs::add<cp::Control>(eid, roomIndex) = ControlSystem::INDEX_P1;
            ecs::add<cp::Orientation>(eid, roomIndex);
            ecs::add<cp::Velocity>(eid, roomIndex).set(80.0, 0.3, 0.2);
            ecs::add<cp::Input>(eid, roomIndex);
            ecs::add<cp::Melee>(eid, roomIndex).set("atk", MeleeComponent::DIR, 12);
            ecs::add<cp::Health>(eid, roomIndex).set(20);
        }),
        NULL));
    cpRender.container->runAction(Sequence::create(
        DelayTime::create(duration / 2),
        FadeTo::create(duration / 4, 255),
        NULL
    ));
    
    view->setOpacity(255);
}

void RoomSystemCtrl::loadRoom(RoomLayer *view, RoomData *data)
{
    collisionSystem.init(data);
    renderSystem.init(data);
    ecsGroup.setID(data->index);
    
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
        unsigned destRoomIndex = gateMap.second.roomIndex;
        unsigned destGateIndex = gateMap.second.gateIndex;
        
        auto eid = cp::entity::genID();
        ecs::add<cp::Gate>(eid, roomIndex).set(
            destRoomIndex, destGateIndex, data->getModel()->gates[srcGateIndex]);
    }
}