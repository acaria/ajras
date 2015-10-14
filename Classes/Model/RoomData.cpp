#include "RoomData.h"
#include "CoreLib.h"

RoomData::RoomData(unsigned roomIndex, RoomModel* model) :
    model(model), grid(model->grid),
    index(roomIndex), type(RoomType::COMMON)
{
    this->depth = 0;
    
    //cross areas
    this->crossAreas = std::list<GateInfo>(model->crossAreas);
    
    //sleep zones
    sleepZones[AIComponent::eSleep::BIRD] = std::list<SleepZone>();
    sleepZones[AIComponent::eSleep::HUMAN] = std::list<SleepZone>();
    for(auto pair : model->sleepZones)
    {
        for(auto bounds : pair.second)
        {
            sleepZones[pair.first].push_back(SleepZone{
                .bounds = bounds,
                .taken = false
            });
        }
    }
}

RoomData::SleepZone* RoomData::getSleepZone(AIComponent::eSleep cat,
                                            const cc::Point& pos)
{
    SleepZone* result = nullptr;
    float dist = 0;
    for(SleepZone& sleepZone : this->sleepZones[cat])
    {
        if (sleepZone.taken)
            continue;
        auto newDist = (pos - cc::Point{sleepZone.bounds.getMidX(),
                                        sleepZone.bounds.getMidY()}).getLength();
        if (dist == 0 || newDist < dist)
        {
            dist = newDist;
            result = &sleepZone;
        }
    }
    return result;
}

void RoomData::freeSleepZone(AIComponent::eSleep cat, const cc::Point &pos)
{
    for(SleepZone& sleepZone : this->sleepZones[cat])
    {
        if (!sleepZone.taken)
            continue;
        
        if (sleepZone.bounds.containsPoint(pos))
            sleepZone.taken = false;
    }
}

cc::Rect RoomData::getBounds()
{
    return cocos2d::Rect(position.x,
                         position.y,
                         model->totalSize.x,
                         model->totalSize.y);
}

cc::Vec2 RoomData::getPosFromCoord(const lib::v2u& coord)
{
    return model->getPosFromCoord(coord);
}

lib::v2u RoomData::getCoordFromPos(const cc::Vec2& pos)
{
    return model->getCoordFromPos(pos);
}

std::vector<ObjectInfo> RoomData::getModelObjs()
{
    return model->objs;
}

lib::v2u RoomData::getDim()
{
    return {grid.width, grid.height};
}

lib::DataGrid<BlockInfo>& RoomData::getContent()
{
    return this->grid;
}

lib::DataGrid<BlockInfo>& RoomData::getGrid()
{
    return this->grid;
}

cc::Size RoomData::getTileSize()
{
    return {(float)this->model->tileSize.x, (float)this->model->tileSize.y};
}

int RoomData::getZOrder(const cc::Vec2& pos)
{
    return this->model->getZOrder(pos);
}

cc::Rect RoomData::getBlockBound(lib::v2u coord)
{
    return this->model->getRectCoord(coord);
}

std::list<cc::Rect> RoomData::getWalls()
{
    return this->model->walls;
}

void RoomData::extractGateAnimInfo(unsigned gateIndex, cc::Rect colRect,
                                   cc::Point& srcPos, cc::Point& destPos)
{
    cocos2d::Vec2 movePos;
    auto gate = this->gateMapping[gateIndex];
    
    switch(gate.info.type)
    {
        case GateInfo::Left:
            srcPos = {gate.info.rect.getMinX() - colRect.getMinX(),
                gate.info.rect.getMinY() + (gate.info.rect.size.height - colRect.size.height) / 2 - colRect.getMinY()};
            movePos = {gate.info.rect.size.width,0};
            break;
        case GateInfo::Right:
            srcPos = {gate.info.rect.getMaxX() - colRect.size.width - colRect.getMinX(),
                gate.info.rect.getMinY() + (gate.info.rect.size.height - colRect.size.height) / 2 - colRect.getMinY()};
            movePos = {-gate.info.rect.size.width,0};
            break;
        case GateInfo::Up:
            srcPos = {gate.info.rect.getMinX() + (gate.info.rect.size.width - colRect.size.width) / 2 - colRect.getMinX(),
                gate.info.rect.getMaxY() - colRect.size.height - colRect.getMinY()};
            movePos = {0,-gate.info.rect.size.height};
            break;
        case GateInfo::Down:
            srcPos = {gate.info.rect.getMinX() + (gate.info.rect.size.width - colRect.size.width) / 2 - colRect.getMinX(),
                gate.info.rect.getMinY() - colRect.getMinY()};
            movePos = {0,gate.info.rect.size.height};
            break;
        default:
            Log("2: unsupported gate type detected");
            break;
    }
    
    destPos = srcPos + movePos;
}
