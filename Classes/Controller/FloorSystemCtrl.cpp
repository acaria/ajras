#include "Headers.h"

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
            this->roomViews[nextRoomIndex]->main->addChild(ecs::get<cp::Render>(eid).container);
        }
    }
    
    if (changeView)
    {
        this->gView->interface->clearTarget();
        this->currentRoomIndex = nextRoomIndex;
        ecsGroup.setID(this->currentRoomIndex);
        
        auto dataRoom = data->getRoomAt(nextRoomIndex);
        this->controlSystem.changeRoom(dataRoom);
        
        //move camera
        auto bounds = dataRoom->getBounds();
        this->gView->moveCamera({bounds.getMidX(), bounds.getMidY()}, 1);
    }
    
    return data->getRoomAt(nextRoomIndex);
}

void FloorSystemCtrl::load(GameScene *gview, MapData *data)
{
    this->clear();
    this->currentRoomIndex = data->getStartRoomIdx();
    this->gView = gview;
    this->data = data;
    this->ecsGroup.setID(this->currentRoomIndex);
    this->controlSystem.init(gview,
                             data->rooms[this->currentRoomIndex]);
    
    cc::Rect bounds = Rect::ZERO;
    for(auto pair : data->rooms)
    {
        auto roomIndex = pair.first;
        auto roomData = pair.second;
        
        auto roomSystemCtrl = new RoomSystemCtrl();
        auto roomLayer = RoomLayer::create();
        
        roomSystemCtrl->load(roomLayer, roomData);
        
        this->roomViews[roomIndex] = roomLayer;
        this->roomSystems[roomIndex] = roomSystemCtrl;
        
        roomLayer->setPosition(roomData->position);
        this->gView->frame->addChild(roomLayer, 1);
        
        bounds = bounds.unionWithRect(roomData->getBounds());
    }
    
    for(auto j = 0; j < bounds.size.height; j+=kBlockSize)
    for(auto i = 0; i < bounds.size.width; i+=kBlockSize)
    {
        auto floorTile = Sprite::createWithSpriteFrameName("floor_tile_fill_1.png");
        floorTile->setAnchorPoint({0,0});
        floorTile->setPosition({bounds.origin.x + i, bounds.origin.y + j});
        this->gView->frame->addChild(floorTile, 0);
    }
    
    auto camRect = data->rooms[this->currentRoomIndex]->getBounds();
    gview->setCamera({camRect.getMidX(), camRect.getMidY()});
}