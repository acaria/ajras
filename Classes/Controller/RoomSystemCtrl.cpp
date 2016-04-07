#include "RoomSystemCtrl.h"
#include "RoomData.h"
#include "LayeredContainer.h"
#include "Components.h"
#include "ModelProvider.h"
#include "MissionScene.h"
#include "HealthBar.h"
#include "GateMap.h"
#include "SysHelper.h"
#include "Defines.h"

RoomSystemCtrl::RoomSystemCtrl(unsigned group,
                               LayeredContainer* view,
                               RoomData* data,
                               SystemDispatcher& dispatcher):
        random(Randgine::instance()->get(Randgine::MISSION)),
        dispatcher(dispatcher)
{
    ecsGroup.setID(group);
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
                auto rl = def::LayerType::BG;
                if (properties.find(BlockInfo::collision) != properties.end() &&
                    properties[BlockInfo::collision] == "flyable")
                {
                    rl = def::LayerType::MAIN1;
                }
                
                auto coord = data->getPosFromCoord({i,j});
                auto sprite = new SpriteEx();
                sprite->initWithSpriteFrameName(properties[BlockInfo::bgTileName]);
                sprite->setPosition(coord);
                view->add(sprite, rl);
                sprite->release();
            }
            
            if (properties.find(BlockInfo::fgTileName) != properties.end())
            {
                auto coord = data->getPosFromCoord({i,j});
                auto sprite = new SpriteEx();
                sprite->initWithSpriteFrameName(properties[BlockInfo::fgTileName]);
                sprite->setPosition(coord);
                
                view->add(sprite, def::LayerType::FG);
                sprite->release();
            }
        }
    
    //objects
    for(auto obj : data->getModelObjs())
    {
        if (lib::hasKey(obj.properties, "profile"))
        {
            SysHelper::createEntity(view, ecsGroup.getID(),
                                    obj.pos, obj.properties["profile"],
                                    random, dispatcher);
            //ecs::add<cp::Control>(eid, data->index) = 2;
        }
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
        auto sprite = new SpriteEx();
        sprite->initWithSpriteFrameName(gateMap.tileName + ".png");
        sprite->setPosition(pos);
        view->add(sprite, def::LayerType::BG);
        sprite->release();
        
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
}

void RoomSystemCtrl::loadZoneObject(const std::string &zoneType, const cc::Rect &bounds, RoomData *data, LayeredContainer *view)
{
    std::string profileName;
    
    bool process = true;
    switch(lib::hash(zoneType))
    {
        case lib::hash("foe"): {
            profileName = "zomb";
            break;
        }
        case lib::hash("reward"): {
            profileName = "chest1";
            break;
        }
        case lib::hash("light"): {
            profileName = "torch1";
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
        assert(profile->stats != nullptr && profile->stats->physics != nullptr);
        auto& colRect = profile->stats->physics->bounds;
        cc::Point pos = {
            bounds.origin.x + (int)random.interval(0.0f, bounds.size.width - colRect.getMaxX()),
            bounds.origin.y + (int)random.interval(0.0f, bounds.size.height - colRect.getMaxY())
        };
        SysHelper::createEntity(view, ecsGroup.getID(),
                                pos, profileName,
                                random, dispatcher);
    }
}

void RoomSystemCtrl::hideObjects(float duration)
{
    for(auto eid : ecsGroup.system<cp::Render>())
    {
        auto& cpRender = ecs::get<cp::Render>(eid);
     
        if (!cpRender.displayed)
            continue;
        cpRender.displayed = false;

        if (duration == 0)
            cpRender.sprite->setOpacity(0);
        else
            cpRender.sprite->runAction(cc::FadeOut::create(duration));
    }
    
    for(auto eid : ecsGroup.system<cp::Light>())
    {
        auto& cpLight = ecs::get<cp::Light>(eid);
        cpLight.halo->stopAllActions();
        if (duration == 0)
            cpLight.halo->setOpacity(0);
        else
            cpLight.halo->runAction(cc::FadeOut::create(duration));
    }
}

void RoomSystemCtrl::showObjects(float duration)
{
    for(auto eid : ecsGroup.system<cp::Render>())
    {
        auto& cpRender = ecs::get<cp::Render>(eid);
        
        if (cpRender.displayed)
            continue;
        cpRender.displayed = true;
        
        cpRender.sprite->stopAllActions();
        if (duration == 0)
            cpRender.sprite->setOpacity(255);
        else
            cpRender.sprite->runAction(cc::FadeIn::create(duration));
    }
    
    for(auto eid : ecsGroup.system<cp::Light>())
    {
        auto& cpLight = ecs::get<cp::Light>(eid);
        if (duration == 0)
        {
            cpLight.halo->setOpacity(cpLight.defaultOpacity.first);
            cpLight.halo->runAction(cpLight.createFlickerAction());
        }
        else
        {
            cpLight.halo->runAction(cc::Sequence::create(
                cc::FadeTo::create(duration, cpLight.defaultOpacity.first),
                cpLight.createFlickerAction(),
                NULL));
        }
    }
}