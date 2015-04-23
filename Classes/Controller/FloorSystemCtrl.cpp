#include "FloorSystemCtrl.h"
#include "Components.h"
#include "BlockInfo.h"
#include "MapData.h"
#include "RoomData.h"
#include "GameScene.h"
#include "InterfaceLayer.h"
#include "NodeRenderer.h"

using namespace std::placeholders;

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
    controlSystem.tick(dt);
    
    //processing only current room
    roomSystems[currentRoomIndex]->tick(dt);
}

void FloorSystemCtrl::animate(double dt, double tickPercent)
{
    controlSystem.animate(dt, tickPercent);
    
    //processing only current room
    roomSystems[currentRoomIndex]->animate(dt, tickPercent);
}

RoomData* FloorSystemCtrl::changeRoom(unsigned int nextRoomIndex,
                                      unsigned int gateIndex,
                                      const std::vector<unsigned int> &eids)
{
    unsigned prevRoomIndex = this->currentRoomIndex;
    
    bool changeView = false;
    for(auto eid : eids)
    {
        cp::entity::move(eid, prevRoomIndex, nextRoomIndex);
        if (ecs::has<cp::Control>(eid) &&
            ecs::get<cp::Control>(eid) == ControlSystem::INDEX_P1)
            changeView = true;
        if (ecs::has<cp::Render>(eid))
        {
            ecs::get<cp::Render>(eid).container->removeFromParentAndCleanup(false);
            auto layer = ecs::get<cp::Render>(eid).chooseLayer(
                this->roomViews[nextRoomIndex]);
            layer->addChild(ecs::get<cp::Render>(eid).container);
        }
    }
    
    if (changeView)
    {
        this->roomSystems[prevRoomIndex]->hideObjects(1);
        this->gView->interface->clearTarget();
        this->currentRoomIndex = nextRoomIndex;
        ecsGroup.setID(this->currentRoomIndex);
        
        auto dataRoom = data->getRoomAt(nextRoomIndex);
        this->controlSystem.changeRoom(dataRoom);
        
        //move camera
        auto bounds = dataRoom->getBounds();
        this->gView->moveCamera({bounds.getMidX(), bounds.getMidY()}, 1);
        this->showRoom(nextRoomIndex, nullptr);
    }
    
    return data->getRoomAt(nextRoomIndex);
}

void FloorSystemCtrl::onHealthChanged(unsigned int roomIndex, unsigned int eid, int health)
{
    
}

void FloorSystemCtrl::displayDebug(GameScene *view, MapData *data)
{
    for(auto pair : data->rooms)
    {
        //auto roomIndex = pair.first;
        auto roomData = pair.second;
        auto bounds = roomData->getBounds();
        
        auto grid = roomData->getModel()->grid;
        
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
                if (roomData->type == RoomData::RoomType::START)
                    pxl->setColor(cc::Color3B::RED);
                else if (roomData->type == RoomData::RoomType::END)
                    pxl->setColor(cc::Color3B::GREEN);
                else
                    pxl->setColor(cc::Color3B::YELLOW);
            }
            
            view->frame->addChild(pxl);
        }
        
        auto txt = cc::Label::createWithTTF(std::to_string(roomData->depth), "fonts/04b03.ttf", 8);
        txt->setColor(cc::Color3B::GREEN);
        txt->setPosition({bounds.getMidX() / 4,
                          bounds.getMidY() / 4});
        view->frame->addChild(txt);
    }
}

void FloorSystemCtrl::start()
{
    auto camRect = data->rooms[this->currentRoomIndex]->getBounds();
    this->gView->setCamera({camRect.getMidX(), camRect.getMidY()});
    
    this->showRoom(this->currentRoomIndex, [this](){
        float duration = 3.0f;
        
        auto roomData = this->data->rooms[this->currentRoomIndex];
        auto roomIndex = roomData->index;
        
        auto warpInfo = roomData->getModel()->warps.front();
        auto srcPos = warpInfo.getSrcPos();
        auto destPos = warpInfo.getDestPos();
        
        for(auto eid : ecs::join<cp::Control, cp::Render, cp::Collision>(roomData->index))
        {
            auto& cpRender = ecs::get<cp::Render>(eid);
            auto& cpCollision = ecs::get<cp::Collision>(eid);
            
            cpRender.container->runAction(cc::Sequence::create(
                cc::MoveTo::create(duration, {
                    destPos.x - cpCollision.rect.getMinX() - cpCollision.rect.size.width / 2,
                    destPos.y - cpCollision.rect.getMinY() - cpCollision.rect.size.height / 2
                }),
                cc::CallFunc::create([eid, roomIndex, cpRender](){
                    ecs::add<cp::Position>(eid, roomIndex).set(cpRender.container->getPosition());
                    ecs::add<cp::Input>(eid, roomIndex);
                }),
                NULL
            ));
            cpRender.container->runAction(cc::Sequence::create(
                cc::DelayTime::create(duration / 2),
                cc::FadeTo::create(duration / 4, 255),
                NULL
            ));
        }

    });
}

void FloorSystemCtrl::showRoom(unsigned int roomIndex, std::function<void()> after)
{
    assert(lib::hasKey(this->roomViews, roomIndex));
    
    if (!lib::hasKey(this->roomPreviews, roomIndex))
    {
        this->roomSystems[roomIndex]->showObjects(1);
        if (after != nullptr)
            after();
    }
    else
    {
        auto preview = this->roomPreviews[roomIndex];
        this->roomPreviews.erase(roomIndex);
        
        auto view = this->roomViews[roomIndex];
        view->setPosition(preview->getPosition());
        
        preview->getSprite()->runAction(cc::Sequence::create(
            cc::FadeTo::create(1, 255),
            cc::CallFunc::create([this, view, preview, roomIndex](){
                this->gView->frame->removeChild(preview);
                this->gView->frame->addChild(view);
                view->release();
                this->roomSystems[roomIndex]->showObjects(1);
            }),
            cc::CallFunc::create(after),
            NULL
        ));
    }
}

void FloorSystemCtrl::load(GameScene *gview,
                           MapData *data)
{
    this->clear();
    this->currentRoomIndex = data->getCurIdxRoom();
    this->gView = gview;
    this->gView->setBgColor(data->getBgColor());
    this->data = data;
    this->ecsGroup.setID(this->currentRoomIndex);
    this->controlSystem.init(gview,
                             data->rooms[this->currentRoomIndex]);
    
    cc::Rect bounds = cc::Rect::ZERO;
    for(auto pair : data->rooms)
    {
        auto roomIndex = pair.first;
        auto roomData = pair.second;
        
        auto roomSystemCtrl = new RoomSystemCtrl();
        roomSystemCtrl->onHealthChanged.registerObserver(std::bind(&FloorSystemCtrl::onHealthChanged, this, _1, _2, _3));
        
        auto roomLayer = RoomLayer::create();
        roomLayer->retain();
        
        roomSystemCtrl->loadRoom(roomLayer, roomData);
        roomSystemCtrl->hideObjects(0);
        
        this->roomViews[roomIndex] = roomLayer;
        this->roomSystems[roomIndex] = roomSystemCtrl;

        auto preview = roomLayer->getShot(
            roomData->getBounds().size.width, roomData->getBounds().size.height);
        this->roomPreviews[roomIndex] = preview;
        
        preview->setPosition(roomData->position);
        preview->getSprite()->setOpacity(0);
        
        this->gView->frame->addChild(preview, 1);
        preview->setOpacity(0);
        
        bounds = bounds.unionWithRect(roomData->getBounds());
    }
    
    for(auto j = 0; j < bounds.size.height; j+=kBlockSize)
    for(auto i = 0; i < bounds.size.width; i+=kBlockSize)
    {
        if (lib::randf01() > 0.1)
            continue;
        auto bgName = *lib::selectRand(data->getBgTiles().begin(), data->getBgTiles().end());
        auto floorTile = cc::Sprite::createWithSpriteFrameName(bgName);
        floorTile->setAnchorPoint({0,0});
        floorTile->setPosition({bounds.origin.x + i, bounds.origin.y + j});
        this->gView->frame->addChild(floorTile, 0);
    }
}