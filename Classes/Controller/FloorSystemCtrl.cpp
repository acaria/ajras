#include "FloorSystemCtrl.h"
#include "Components.h"
#include "BlockInfo.h"
#include "FloorData.h"
#include "RoomData.h"
#include "MissionScene.h"
#include "MissionInterface.h"
#include "NodeRenderer.h"
#include "HealthBar.h"
#include "GateMap.h"
#include "GameCtrl.h"
#include "RenderComponent.h"
#include "PlayerData.h"

using namespace std::placeholders;

FloorSystemCtrl::FloorSystemCtrl() : ctrlMissionSystem(ecsGroup),
                                     random(Randgine::instance()->get(Randgine::FLOOR)),
                                     playerFocus(nullptr)
{
    
}

void FloorSystemCtrl::clear()
{
    for(auto pair : this->roomSystems)
    {
        if (pair.second != nullptr)
        {
            delete pair.second;
        }
    }
    this->roomSystems.clear();
    this->roomViews.clear();
}

FloorSystemCtrl::~FloorSystemCtrl()
{
    this->clear();
}

void FloorSystemCtrl::tick(double dt)
{
    ctrlMissionSystem.tick(dt);
    
    //processing only current room
    roomSystems[currentRoomIndex]->tick(dt);
}

void FloorSystemCtrl::animate(double dt, double tickPercent)
{
    //focus entity
    if (playerFocus->entityFocus != 0 && ecs::has<cp::Render>(playerFocus->entityFocus))
    {
        auto& cpRender = ecs::get<cp::Render>(playerFocus->entityFocus);
        auto pos = this->data->rooms[this->currentRoomIndex]->position +
            cpRender.sprite->getPosition() + cpRender.sprite->getContentSize() / 2;
        this->gView->getCam()->focusTarget(pos);
    }

    ctrlMissionSystem.animate(dt, tickPercent);
    
    //processing only current room
    roomSystems[currentRoomIndex]->animate(dt, tickPercent);
}

void FloorSystemCtrl::registerEvents(RoomSystemCtrl *ctrl)
{
    this->eventRegs.push_back(ctrl->onHealthChanged.registerObserver(
        std::bind(&FloorSystemCtrl::onHealthChanged, this, _1, _2, _3)));
    
    this->eventRegs.push_back(ctrl->onGateTriggered.registerObserver([this](
        unsigned prevRoomIndex, unsigned eid, GateMap  gate){
        switch(gate.cmd)
        {
            case GateMap::CmdType::CHANGE_ROOM:
                this->onRoomChanged(prevRoomIndex, eid, gate);
                break;
            case GateMap::CmdType::ENTER_MAP:
                this->onFloorStart();
                break;
            case GateMap::CmdType::EXIT_MAP:
                this->onFloorFinish();
                break;
            default:
                Log("invalid comaand type in FloorSystemCtrl::registerEvents");
                break;
        }
    }));
}

void FloorSystemCtrl::onFloorStart()
{
    GameCtrl::instance()->goToMainMenu();
}

void FloorSystemCtrl::onFloorFinish()
{
    GameCtrl::instance()->goToMainMenu();
}

void FloorSystemCtrl::onRoomChanged(unsigned prevRoomIndex,
                                      unsigned eid,
                                      GateMap  gate)
{
    unsigned nextRoomIndex = gate.destRoomIndex;

    cp::entity::move(eid, prevRoomIndex, nextRoomIndex);

    if (ecs::has<cp::Render>(eid))
    {
        ecs::get<cp::Render>(eid).sprite->removeFromParentAndCleanup(false);
        auto layer = ecs::get<cp::Render>(eid).chooseLayer(
            this->roomViews[nextRoomIndex]);
        layer->addChild(ecs::get<cp::Render>(eid).sprite);
    }
    
    auto nextRoom = data->getRoomAt(nextRoomIndex);
    
    //gate introduction
    float duration = 1.0f;
    
    auto& render = ecs::get<cp::Render>(eid);
    cocos2d::Vec2 srcPos, destPos;
    
    nextRoom->extractGateAnimInfo(gate.destGateIndex,
                                  ecs::get<cp::Collision>(eid).rect,
                                  /*out*/srcPos,
                                  /*out*/destPos);
    render.sprite->setPosition(srcPos);
    render.sprite->runAction(cc::Sequence::create(
        cc::MoveBy::create(duration, destPos - srcPos),
        cc::CallFunc::create([eid, destPos, nextRoomIndex](){
            ecs::get<cp::Input>(eid).forceEnable();
            ecs::add<cp::Position>(eid, nextRoomIndex).set(destPos);
        }),
        NULL
    ));
    render.sprite->runAction(cc::Sequence::create(
        cc::DelayTime::create(duration / 2),
        cc::TintTo::create(duration / 2, cc::Color3B::WHITE),
        NULL
    ));
    
    if (eid == playerFocus->entityFocus) //change room
    {
        this->roomSystems[prevRoomIndex]->hideObjects(1);
        this->gView->interface->clearTarget();
        this->currentRoomIndex = nextRoomIndex;
        ecsGroup.setID(this->currentRoomIndex);
        
        auto dataRoom = data->getRoomAt(nextRoomIndex);
        this->ctrlMissionSystem.changeRoom(dataRoom);
        
        //move camera
        auto bounds = dataRoom->getBounds();
        this->gView->getCam()->moveTarget(destPos + bounds.origin, 1);
        this->showRoom(nextRoomIndex, nullptr);

        render.sprite->runAction(cc::Sequence::create(cc::DelayTime::create(duration / 2),
            cc::FadeTo::create(duration / 2, 255),
            NULL
        ));
    }
}

void FloorSystemCtrl::onHealthChanged(unsigned int roomIndex, unsigned int eid, int health)
{
    if (health == 0)
    {
        this->gView->interface->unsetTargetID(eid);
    }
    if (eid == playerFocus->entityFocus)
    {
        this->gView->interface->getHealthBar()->updateProperties(health);
    }
}

cc::Sprite* FloorSystemCtrl::displayMap(FloorData *data)
{
    auto result = Sprite::create();
    
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

void FloorSystemCtrl::displayDebug(MissionScene *view, FloorData *data)
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
            
            view->getFrame()->addChild(pxl);
        }
        
        auto txt = cc::Label::createWithTTF(std::to_string(roomData->depth), "fonts/04b03.ttf", 8);
        txt->setColor(cc::Color3B::GREEN);
        txt->setPosition({bounds.getMidX() / 4,
                          bounds.getMidY() / 4});
        view->getFrame()->addChild(txt);
    }
}

void FloorSystemCtrl::start()
{
    this->playerFocus = GameCtrl::instance()->getData().curPlayer();
    auto camRect = data->rooms[this->currentRoomIndex]->getBounds();
    this->gView->getCam()->setTarget({camRect.getMidX(), camRect.getMidY()});
    
    auto roomIndex = this->currentRoomIndex;
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
    assert(enterGateRef != nullptr);
    GateMap enterGate = *enterGateRef;
    
    //show room
    this->showRoom(roomIndex, [this, enterGate]() {
        float duration = 3.0f;
        
        auto roomData = this->data->rooms[this->currentRoomIndex];
        auto roomIndex = roomData->index;
        
        auto srcPos = enterGate.info.getSrcPos();
        auto destPos = enterGate.info.getDestPos();
                
        if (playerFocus->entityFocus != 0 &&
            ecs::has<cp::Render, cp::Collision>(playerFocus->entityFocus))
        {
            unsigned eid = playerFocus->entityFocus;
            auto& cpRender = ecs::get<cp::Render>(eid);
            auto& cpCollision = ecs::get<cp::Collision>(eid);
            
            cpRender.setMoveAnimation(enterGate.info.getDir(), true);
            
            cpRender.sprite->stopAllActions();
            cpRender.sprite->runAction(cc::Sequence::create(
                cc::MoveTo::create(duration, {
                    destPos.x - cpCollision.rect.getMinX() - cpCollision.rect.size.width / 2,
                    destPos.y - cpCollision.rect.getMinY() - cpCollision.rect.size.height / 2
                }),
                cc::CallFunc::create([eid, roomIndex](){
                    auto& cpRender = ecs::get<cp::Render>(eid);
                    ecs::add<cp::Input>(eid, roomIndex);
                    ecs::get<cp::Position>(eid).set(cpRender.sprite->getPosition());
                    cpRender.manualPosMode = false;
                    cpRender.cancelAnimation();
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
    auto profile = GameCtrl::instance()->getData().model.profile.get("boy");
    auto srcPos = enterGate.info.getSrcPos();
    
    auto& cpRender = ecs::add<cp::Render>(eid, roomIndex);
    auto& cpCollision = ecs::add<cp::Collision>(eid, roomIndex);
    
    cpRender.setProfile(profile,
                        RenderComponent::chooseLayer(profile, roomView),
                        roomData->getZOrder(srcPos));
    cpCollision.setProfile(profile);
    
    ecs::add<cp::AI>(eid, roomIndex).setProfile(profile);
    ecs::add<cp::Velocity>(eid, roomIndex).setProfile(profile);
    ecs::add<cp::Melee>(eid, roomIndex).setProfile(profile);
    ecs::add<cp::Orientation>(eid, roomIndex);
    ecs::add<cp::Control>(eid, roomIndex) = playerFocus->ctrlIndex;
    ecs::add<cp::Gear>(eid, roomIndex).set(playerFocus->inventory);
    
    auto& csHealth = ecs::add<cp::Health>(eid, roomIndex);
    csHealth.setProfile(profile);
    
    cpRender.sprite->setPosition({
        srcPos.x - cpCollision.rect.getMinX() - cpCollision.rect.size.width / 2,
        srcPos.y - cpCollision.rect.getMinY() - cpCollision.rect.size.height / 2
    });
    cpRender.sprite->setOpacity(0);
    cpRender.manualPosMode = true;
    ecs::add<cp::Position>(eid, roomIndex).set(cpRender.sprite->getPosition());
    
    playerFocus->entityFocus = eid;
    this->gView->interface->getHealthBar()->initProperties(csHealth.maxHp,
                                                           csHealth.hp);
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
                this->gView->getFrame()->removeChild(preview);
                this->gView->getFrame()->addChild(view);
                view->release();
                this->roomSystems[roomIndex]->showObjects(0.5);
            }),
            cc::CallFunc::create(after),
            NULL
        ));
    }
}

CtrlMissionSystem* FloorSystemCtrl::getCtrlSystem()
{
    return &this->ctrlMissionSystem;
}

void FloorSystemCtrl::load(MissionScene *gview,
                           FloorData *data)
{
    this->clear();
    this->currentRoomIndex = data->getCurIdxRoom();
    this->gView = gview;
    this->gView->setBgColor(data->getBgColor());
    this->data = data;
    this->ecsGroup.setID(this->currentRoomIndex);
    this->ctrlMissionSystem.init(data->rooms[this->currentRoomIndex]);
    
    cc::Rect bounds = cc::Rect::ZERO;
    for(auto pair : data->rooms)
    {
        auto roomIndex = pair.first;
        auto roomData = pair.second;
        
        auto roomSystemCtrl = new RoomSystemCtrl();
        this->registerEvents(roomSystemCtrl);
        
        auto roomLayer = RoomLayer::create();
        roomLayer->retain();
        
        roomSystemCtrl->loadRoom(roomLayer, roomData);
        roomSystemCtrl->hideObjects(0);
        
        this->roomViews[roomIndex] = roomLayer;
        this->roomSystems[roomIndex] = roomSystemCtrl;

        auto preview = roomLayer->getShot(
            roomData->getBounds().size.width, roomData->getBounds().size.height);
        this->roomPreviews[roomIndex] = preview;
        
        roomLayer->setPosition(roomData->position);
        preview->setPosition(roomData->position);
        preview->getSprite()->setOpacity(0);
        
        this->gView->getFrame()->addChild(preview, 1);
        preview->setOpacity(0);
        
        bounds = bounds.unionWithRect(roomData->getBounds());
    }
    
    //too slow!
    /*auto batch = cc::Sprite::create();
    int count = 0;
    for(auto j = 0; j < bounds.size.height; j+=kBlockSize)
    for(auto i = 0; i < bounds.size.width; i+=kBlockSize)
    {
        if (random.ratio() > 0.1)
            continue;
        auto bgName = random.select(data->getBgTiles());
        auto floorTile = cc::Sprite::createWithSpriteFrameName(bgName);
        floorTile->setAnchorPoint({0,0});
        floorTile->setPosition({bounds.origin.x + i, bounds.origin.y + j});
        batch->addChild(floorTile);
        count++;
    }
    this->gView->frame->addChild(batch);
    */
}