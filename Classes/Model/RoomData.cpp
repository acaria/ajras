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
    sleepZones["bird"] = std::list<def::SleepZone>();
    sleepZones["human"] = std::list<def::SleepZone>();
    for(auto pair : model->sleepZones)
    {
        for(auto bounds : pair.second)
        {
            sleepZones[pair.first].push_back({
                .bounds = bounds,
                .taken = false
            });
        }
    }
}

void RoomData::init()
{
    //gates
    for(auto& pair : gateMapping)
    {
        const auto& gate = pair.second;
        cc::Point pos = {gate.info.rect.origin.x, gate.info.rect.origin.y};
        
        //change collision data
        
        auto gateSrcCoord = getCoordFromPos(pos);
        auto gateDestCoord = getCoordFromPos({
            gate.info.rect.getMaxX(),
            gate.info.rect.getMaxY()});
        
        for(unsigned j = gateSrcCoord.y; j < gateDestCoord.y; j++)
        for(unsigned i = gateSrcCoord.x; i < gateDestCoord.x; i++)
        {
            grid[{i, j}].fields[BlockInfo::collision] = "walkable";
            grid[{i, j}].fields[BlockInfo::gating] = "gate";
        }
    }
    
    this->collision.init(this);
    this->navigation.init(this);
}

CollisionInfo* RoomData::getCol()
{
    return &collision;
}

NavigationInfo* RoomData::getNav()
{
    return &navigation;
}

def::SleepZone* RoomData::getSleepZone(const std::string& cat,
                                            const cc::Point& pos)
{
    def::SleepZone* result = nullptr;
    float dist = 0;
    for(auto& sleepZone : this->sleepZones[cat])
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

void RoomData::freeSleepZone(const std::string& cat, const cc::Point &pos)
{
    for(auto& sleepZone : this->sleepZones[cat])
    {
        if (!sleepZone.taken)
            continue;
        
        if (sleepZone.bounds.containsPoint(pos))
            sleepZone.taken = false;
    }
}

cc::Rect RoomData::getBounds()
{
    return cocos2d::Rect(position.x, position.y, model->totalSize.width, model->totalSize.height);
}

cc::Vec2 RoomData::getPosFromCoord(const lib::v2u& coord)
{
    return model->getPosFromCoord(coord);
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
    return {this->model->tileSize.width, this->model->tileSize.height};
}

cc::Rect RoomData::getBlockBound(const lib::v2u& coord)
{
    return this->model->getRectCoord(coord);
}

lib::v2u RoomData::getCoordFromPos(const cc::Vec2& pos)
{
    return this->model->getCoordFromPos(pos);
}

std::list<cc::Rect> RoomData::getWalls()
{
    return this->model->walls;
}

GateMap RoomData::getEnterGate()
{
    for(auto element : gateMapping)
    {
        if (element.second.cmd == GateMap::CmdType::ENTER_MAP)
            return element.second;
    }
    CCASSERT(false, "enter gate is missing");
    return GateMap();
}

std::pair<cc::Point, cc::Point> RoomData::extractGateAnimInfo(const GateMap& srcGate, cc::Rect colRect)
{
    cc::Point srcPos, movePos;
    
    GateMap gate;
    switch(srcGate.cmd)
    {
        case GateMap::CmdType::CHANGE_ROOM:
            gate = this->gateMapping[srcGate.destGateIndex];
            break;
        case GateMap::CmdType::ENTER_MAP:
            gate = srcGate;
            break;
        default:
            CCASSERT(false, "not implemented");
            break;
    }
    
    auto r = gate.info.rect;
    switch(gate.info.type)
    {
        case GateInfo::Left:
            srcPos = {r.getMinX() - colRect.getMinX(),
                r.getMinY() + (r.size.height - colRect.size.height) / 2 - colRect.getMinY()};
            movePos = {r.size.width,0};
            break;
        case GateInfo::Right:
            srcPos = {r.getMaxX() - colRect.size.width - colRect.getMinX(),
                r.getMinY() + (r.size.height - colRect.size.height) / 2 - colRect.getMinY()};
            movePos = {-r.size.width,0};
            break;
        case GateInfo::Up:
            srcPos = {r.getMinX() + (r.size.width - colRect.size.width) / 2 - colRect.getMinX(),
                r.getMaxY() - colRect.size.height - colRect.getMinY()};
            movePos = {0,-r.size.height};
            break;
        case GateInfo::Down:
            srcPos = {r.getMinX() + (r.size.width - colRect.size.width) / 2 - colRect.getMinX(),
                r.getMinY() - colRect.getMinY()};
            movePos = {0,r.size.height};
            break;
        default:
            CCASSERT(false, "2: unsupported gate type detected");
            break;
    }
    
    return {srcPos, srcPos + movePos};
}
