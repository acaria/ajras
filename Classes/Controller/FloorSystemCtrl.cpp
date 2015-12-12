#include "FloorSystemCtrl.h"
#include "Components.h"
#include "BlockInfo.h"
#include "LayeredContainer.h"
#include "FloorData.h"
#include "RoomData.h"
#include "MissionScene.h"
#include "MissionInterface.h"
#include "NodeRenderer.h"
#include "HealthBar.h"
#include "ModelProvider.h"
#include "RenderComponent.h"
#include "PlayerData.h"

using namespace std::placeholders;

FloorSystemCtrl::FloorSystemCtrl() : systemFacade(dispatcher, context, Randgine::CAT::MISSION)
{
    //init group
    this->context.ecs = &ecsGroup;

    //init systems
    this->systemFacade.factory<ControlSystem>(
        std::list<unsigned>({PlayerData::ctrlIndex, PlayerData::debugIndex}));
    this->systemFacade.factory<AISystem>();
    this->systemFacade.factory<UpdaterSystem>();
    this->systemFacade.factory<TargetSystem>();
    this->systemFacade.factory<MoveSystem>();
    this->systemFacade.factory<MeleeSystem>();
    this->systemFacade.factory<TransitSystem>();
    this->systemFacade.factory<CollisionSystem>();
    this->systemFacade.factory<RenderSystem>();
    this->systemFacade.factory<InteractSystem>();
#if ECSYSTEM_DEBUG
    this->systemFacade.factory<DebugSystem>();
#endif
}

FloorSystemCtrl::~FloorSystemCtrl()
{
    this->clear();
}

void FloorSystemCtrl::clear()
{
    this->eventRegs.clear();
    for(auto pair : this->roomSystems)
    {
        cp::entity::clear(pair.first);
        if (pair.second != nullptr)
        {
            delete pair.second;
        }
    }
    this->roomSystems.clear();
    this->roomViews.clear();
}

void FloorSystemCtrl::tick(double dt)
{
    systemFacade.tick(dt);
}

void FloorSystemCtrl::animate(double dt, double tickPercent)
{
    //focus entity
    if (playerData->entityFocus != 0 && ecs::has<cp::Render>(playerData->entityFocus))
    {
        auto& cpRender = ecs::get<cp::Render>(playerData->entityFocus);
        auto pos = this->data->getCurrentRoom()->position +
            cpRender.sprite->getPosition() + cpRender.sprite->getContentSize() / 2;
        this->cam->focusTarget(pos);
    }

    systemFacade.animate(dt, tickPercent);
}

SystemDispatcher& FloorSystemCtrl::getDispatcher()
{
    return dispatcher;
}

void FloorSystemCtrl::onRoomChanged(unsigned prevRoomIndex,
                                    unsigned eid,
                                    GateMap  gate)
{
    unsigned nextRoomIndex = gate.destRoomIndex;

    dispatcher.onEntityDeleted(prevRoomIndex, eid);
    cp::entity::move(eid, prevRoomIndex, nextRoomIndex);

    if (ecs::has<cp::Render>(eid))
    {
        auto& sprite = ecs::get<cp::Render>(eid).sprite;
        auto container = this->roomViews[nextRoomIndex];
        if (sprite->getParent() != nullptr)
        {
            auto lType = static_cast<def::LayerType>(sprite->getParent()->getTag());
            sprite->removeFromParentAndCleanup(false);
            container->add(sprite, lType);
        }
        else //orphelin, cannot retrieve layertype
        {
            container->add(sprite, def::LayerType::BG);
        }
    }
    
    auto nextRoom = data->getRoomAt(nextRoomIndex);
    
    //gate introduction
    float duration = 1.0f;
    
    auto& render = ecs::get<cp::Render>(eid);
    cocos2d::Vec2 srcPos, destPos;
    
    nextRoom->extractGateAnimInfo(gate.destGateIndex,
                                  ecs::get<cp::Physics>(eid).shape,
                                  /*out*/srcPos,
                                  /*out*/destPos);
    render.sprite->setPosition(srcPos);
    render.sprite->runAction(cc::Sequence::create(
        cc::MoveBy::create(duration, destPos - srcPos),
        cc::CallFunc::create([eid, nextRoomIndex, this, &render](){
            ecs::add<cp::Position>(eid, nextRoomIndex).set(render.sprite->getPosition());
            dispatcher.onEntityAdded(nextRoomIndex, eid);
        }),
        NULL
    ));
    render.sprite->runAction(cc::Sequence::create(
        cc::DelayTime::create(duration / 2),
        cc::TintTo::create(duration / 2, cc::Color3B::WHITE),
        NULL
    ));
    
    if (eid == playerData->entityFocus) //change room
    {
        this->roomSystems[prevRoomIndex]->hideObjects(1);
        dispatcher.onSystemChanged(nextRoomIndex);
        this->data->setCurIdxRoom(nextRoomIndex);
        
        
        auto dataRoom = data->getRoomAt(nextRoomIndex);
        
        //move camera
        auto bounds = dataRoom->getBounds();
        this->cam->moveTarget(destPos + bounds.origin, 1);
        this->showRoom(nextRoomIndex, nullptr);

        render.sprite->runAction(cc::Sequence::create(cc::DelayTime::create(duration / 2),
            cc::FadeTo::create(duration / 2, 255),
            NULL
        ));
    }
}

/*cc::Sprite* FloorSystemCtrl::displayMap(FloorData *data)
{
    auto result = cc::Sprite::create();
    
    auto pxl = cc::Sprite::createWithSpriteFrameName("pixel.png");
    pxl->setAnchorPoint({0,0});
    pxl->setScale(data->floorMapping->getMappingSize().width / 4,
                  data->floorMapping->getMappingSize().height / 4);
    pxl->setColor(cc::Color3B::ORANGE);
    result->addChild(pxl);
    
    for(auto pair : data->rooms)
    {
        auto dataRoom = pair.second;
        for(auto wall : dataRoom->getWalls())
        {
            auto pxl = cc::Sprite::createWithSpriteFrameName("pixel.png");
            pxl->setAnchorPoint({0,0});
            pxl->setPosition({(dataRoom->position.x + wall.origin.x) / 4,
                (dataRoom->position.y + wall.origin.y) / 4});
            pxl->setScale(wall.size.width / 4, wall.size.height / 4);
            pxl->setColor(cc::Color3B::RED);
            result->addChild(pxl);
        }
        
        for(auto gate : dataRoom->gateMapping)
        {
            auto gateMap = gate.second;
            auto pxl = cc::Sprite::createWithSpriteFrameName("pixel.png");
            pxl->setAnchorPoint({0,0});
            pxl->setPosition({
                (dataRoom->position.x + gateMap.info.rect.origin.x) / 4,
                (dataRoom->position.y + gateMap.info.rect.origin.y) / 4});
            pxl->setScale(gateMap.info.rect.size.width / 4, gateMap.info.rect.size.height / 4);
            if (gateMap.cmd == GateMap::CmdType::ENTER_MAP)
                pxl->setColor(cc::Color3B::BLACK);
            else if (gateMap.cmd == GateMap::CmdType::EXIT_MAP)
                pxl->setColor(cc::Color3B::BLUE);
            else
                pxl->setColor(cc::Color3B::MAGENTA);
            result->addChild(pxl);
        }
        
        auto txt = cc::Label::createWithTTF(std::to_string(dataRoom->depth), "fonts/04b03.ttf", 8);
        txt->setColor(cc::Color3B::GREEN);
        txt->setPosition({dataRoom->getBounds().getMidX() / 4,
                          dataRoom->getBounds().getMidY() / 4});
        result->addChild(txt);
    }

    return result;
}

void FloorSystemCtrl::displayDebug(cc::Node* view, FloorData *data)
{
    for(auto pair : data->rooms)
    {
        //auto roomIndex = pair.first;
        auto roomData = pair.second;
        auto bounds = roomData->getBounds();
        
        auto grid = roomData->getContent();
        
        for(int j = 0; j < grid.height; j++)
        for(int i = 0; i < grid.width; i++)
        {
            auto gridInfo = grid.get(i, j);
            auto pxl = cc::Sprite::createWithSpriteFrameName("pixel.png");
            pxl->setPosition({
                bounds.getMinX() / 4 + i * 4,
                bounds.getMinY() / 4 + j * 4
            });
            pxl->setScale(4.0, 4.0);
            pxl->setColor(cc::Color3B::BLACK);
            
            if (gridInfo.fields[BlockInfo::collision] == "walkable" ||
                gridInfo.fields[BlockInfo::collision] == "flyable")
            {
                if (roomData->type == RoomData::RoomType::STARTER)
                    pxl->setColor(cc::Color3B::RED);
                else if (roomData->type == RoomData::RoomType::FINISHER)
                    pxl->setColor(cc::Color3B::GREEN);
                else
                    pxl->setColor(cc::Color3B::YELLOW);
            }
            
            view->addChild(pxl);
        }
        
        auto txt = cc::Label::createWithTTF(std::to_string(roomData->depth), "fonts/04b03.ttf", 8);
        txt->setColor(cc::Color3B::GREEN);
        txt->setPosition({bounds.getMidX() / 4,
                          bounds.getMidY() / 4});
        view->addChild(txt);
    }
}*/

void FloorSystemCtrl::start()
{
    auto roomIndex = this->data->getCurIdxRoom();
    auto camRect = data->getCurrentRoom()->getBounds();
    this->cam->setTarget({camRect.getMidX(), camRect.getMidY()});
    
    
    auto roomData = this->data->rooms[roomIndex];
    auto roomView = this->roomViews[roomIndex];
    
    //find warp enter
    GateMap* enterGateRef = nullptr;
    for(auto element : roomData->gateMapping)
    {
        if (element.second.cmd == GateMap::CmdType::ENTER_MAP)
        {
            enterGateRef = &element.second;
            break;
        }
    }
    assert(enterGateRef);
    GateMap enterGate = *enterGateRef;
    
    //show room
    this->showRoom(roomIndex, [this, enterGate]() {
        float duration = 3.0f;
        
        auto srcPos = enterGate.info.getSrcPos();
        auto destPos = enterGate.info.getDestPos();
                
        if (playerData->entityFocus != 0 &&
            ecs::has<cp::Render, cp::Physics>(playerData->entityFocus))
        {
            unsigned eid = playerData->entityFocus;
            auto& cpRender = ecs::get<cp::Render>(eid);
            auto& cpCollision = ecs::get<cp::Physics>(eid);
            
            cpRender.busy = true;
            cpRender.setMoveAnimation(enterGate.info.getDir(), true);
            
            cpRender.sprite->stopAllActions();
            cpRender.sprite->runAction(cc::Sequence::create(
                cc::MoveTo::create(duration, {
                    destPos.x - cpCollision.shape.getMinX() - cpCollision.shape.size.width / 2,
                    destPos.y - cpCollision.shape.getMinY() - cpCollision.shape.size.height / 2
                }),
                cc::CallFunc::create([eid, this](){
                    auto& cpRender = ecs::get<cp::Render>(eid);
                    cpRender.cancelAnimation();
                    context.ecs->add<cp::Position>(eid).set(cpRender.sprite->getPosition());
                    dispatcher.onEntityPositionChanged(context.ecs->getID(), eid);
                }),
                NULL
            ));
            cpRender.sprite->runAction(cc::Sequence::create(
                cc::DelayTime::create(duration / 2),
                cc::FadeTo::create(duration / 4, 255),
                NULL
            ));
        }
    });
    
    //create player
    auto eid = cp::entity::genID();
    auto profile = ModelProvider::instance()->profile.get(playerData->charProfileName);
    auto srcPos = enterGate.info.getSrcPos();
    
    auto& cpRender = ecs::add<cp::Render>(eid, roomIndex);
    auto& cpPhy = ecs::add<cp::Physics>(eid, roomIndex);
    
    cpRender.setProfile(profile, roomView);
    cpPhy.setProfile(profile);
    
    cc::Vec2 pos = {
        srcPos.x - cpPhy.shape.getMidX(),
        srcPos.y - cpPhy.shape.getMidY()
    };
    
    ecs::add<cp::Orientation>(eid, roomIndex);
    ecs::add<cp::Mood>(eid, roomIndex) = profile->getMood();
    ecs::add<cp::AI>(eid, roomIndex).setProfile(profile);
    ecs::add<cp::Melee>(eid, roomIndex).setProfile(profile);
    ecs::add<cp::Control>(eid, roomIndex) = playerData->ctrlIndex;
    ecs::add<cp::Gear>(eid, roomIndex) = playerData->inventory;
    ecs::add<cp::Stamina>(eid, roomIndex).setProfile(profile);
    ecs::add<cp::Health>(eid, roomIndex).setProfile(profile);
    ecs::add<cp::Input>(eid, roomIndex);
    
    cpRender.sprite->setPosition(pos);
    cpRender.sprite->setOpacity(0);
    
    playerData->entityFocus = eid;
    dispatcher.onEntityAdded(roomIndex, eid);
}

void FloorSystemCtrl::showRoom(unsigned int roomIndex, std::function<void()> after)
{
    assert(lib::hasKey(this->roomViews, roomIndex));
    
    if (!lib::hasKey(this->roomPreviews, roomIndex))
    {
        this->roomSystems[roomIndex]->showObjects(0.5);
        if (after != nullptr)
            after();
    }
    else
    {
        auto preview = this->roomPreviews[roomIndex];
        this->roomPreviews.erase(roomIndex);
        
        auto view = this->roomViews[roomIndex];
        
        preview->getSprite()->runAction(cc::Sequence::create(
            cc::FadeTo::create(1, 255),
            cc::CallFunc::create([this, view, preview, roomIndex](){
                this->view->removeChild(preview);
                this->view->addChild(view);
                view->release();
                this->roomSystems[roomIndex]->showObjects(0.5);
            }),
            cc::CallFunc::create(after),
            NULL
        ));
    }
}

void FloorSystemCtrl::bindSystems()
{
    //bind events
    this->eventRegs.clear();
    this->eventRegs.push_back(dispatcher.onGateTriggered.registerObserver(
            [this](unsigned prevRoomIndex, unsigned eid, GateMap  gate){
        switch(gate.cmd)
        {
            case GateMap::CmdType::CHANGE_ROOM:
                this->onRoomChanged(prevRoomIndex, eid, gate);
                break;
            default:
                break;
        }
    }));
    
    this->eventRegs.push_back(dispatcher.onSystemChanged.registerObserver(
            [this](unsigned group) {
        context.data = this->data->rooms[group];
        context.view = roomViews[group];
        context.ecs->setID(group);
        dispatcher.onContextChanged();
    }));
}

void FloorSystemCtrl::load(GameCamera *cam, cc::Node *view,
                           PlayerData *player, FloorData *data)
{
    //init context data
    unsigned group = data->getCurIdxRoom();
    
    this->view = view;
    this->cam = cam;
    this->data = data;
    this->playerData = player;
    
    cc::Rect bounds = cc::Rect::ZERO;
    for(auto pair : data->rooms)
    {
        auto roomIndex = pair.first;
        auto roomData = pair.second;
        
        auto layeredNode = cc::create<LayeredContainer>(roomData->getBounds().size);
        layeredNode->retain();
        
        auto roomSystemCtrl = new RoomSystemCtrl(roomIndex, layeredNode, roomData, dispatcher);
        
        roomSystemCtrl->hideObjects(0);
        
        this->roomViews[roomIndex] = layeredNode;
        this->roomSystems[roomIndex] = roomSystemCtrl;

        auto preview = layeredNode->createShot(
            roomData->getBounds().size.width, roomData->getBounds().size.height);
        this->roomPreviews[roomIndex] = preview;
        
        layeredNode->setPosition(roomData->position);
        preview->setPosition(roomData->position);
        preview->getSprite()->setOpacity(0);
        
        this->view->addChild(preview, 1);
        preview->setOpacity(0);
        
        bounds = bounds.unionWithRect(roomData->getBounds());
    }
    
    //too slow!
    /*auto batch = cc::Node::create();
    int count = 0;
    for(auto j = 0; j < bounds.size.height; j+= def::blockSize)
        for(auto i = 0; i < bounds.size.width; i += def::blockSize)
    {
        if (random.ratio() > 0.1)
            continue;
        auto bgName = random.select(data->getBgTiles());
        auto floorTile = cc::Sprite::createWithSpriteFrameName(bgName);
        floorTile->getTexture()->setAliasTexParameters();
        floorTile->setAnchorPoint({0,0});
        floorTile->setPosition({bounds.origin.x + i, bounds.origin.y + j});
        batch->addChild(floorTile);
        count++;
    }
    this->view->addChild(batch);*/
    
    //init context
    this->ecsGroup.setID(group);
    this->context.data = this->data->getCurrentRoom();
    this->context.view = this->roomViews[this->data->getCurIdxRoom()];
    this->bindSystems();
    //systems READY
    dispatcher.onContextChanged();
}