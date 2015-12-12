#include "FloorMapping.h"

//ctors
FloorMapping::FloorMapping(unsigned width, unsigned height, std::map<GateInfo::GateType,
             std::pair<std::string, cc::Rect>> gateConfig):
        size(width, height),
        random(Randgine::instance()->get(Randgine::MAP_DATA)) {
    this->gateConfig = gateConfig;
}

void FloorMapping::addRoom(cc::Point roomPos, std::list<cc::Rect> walls)
{
    for (auto wall : walls)
    {
        cc::Rect absRect = {
            wall.origin.x + roomPos.x, wall.origin.y + roomPos.y,
            wall.size.width, wall.size.height
        };
        takenList.push_back(absRect);
        if (this->totalBounds.size.width == 0 ||
            this->totalBounds.size.height == 0)
            this->totalBounds = absRect;
        else
            this->totalBounds = this->totalBounds.unionWithRect(absRect);
    }
}

cc::Size FloorMapping::getMappingSize()
{
    return size;
}

bool FloorMapping::checkRoom(cc::Point roomPos, RoomModel* model)
{
    bool oneFit = false;
    for(auto wallRef : model->walls)
    {
        cc::Rect newWall = {wallRef.origin.x + roomPos.x,
            wallRef.origin.y + roomPos.y,
            wallRef.size.width,
            wallRef.size.height};
        
        if (newWall.getMinX() < 0 || newWall.getMinY() < 0 ||
            newWall.getMaxX() > this->size.width ||
            newWall.getMaxY() > this->size.height)
            return false;
        
        for(auto wall : takenList)
        {
            if (wall.intersectsRect(newWall))
                return false; //wall collision
        }
        
        for(auto crossingInfo : crossingLeft)
        {
            if (crossingInfo.gateInfo.rect.intersectsRect(newWall))
            {
                bool gateFits = false;
                for(auto newCross : model->crossAreas)
                {
                    cc::Rect refRect = {
                        newCross.rect.origin.x + roomPos.x,
                        newCross.rect.origin.y + roomPos.y,
                        newCross.rect.size.width,
                        newCross.rect.size.height
                    };
                    auto gateRect = gateConfig[crossingInfo.gateInfo.type].second;
                    auto resultRect = lib::getIntersection(crossingInfo.gateInfo.rect,
                                                           refRect);
                    if (resultRect.size.width >= gateRect.size.width &&
                        resultRect.size.height >= gateRect.size.height)
                    {
                        oneFit = true;
                        gateFits = true;
                        break; //gate fits
                    }
                }
                if (!gateFits)
                    return false; //gate collision
            }
        }
    }
    
    if (!oneFit)
        return false;
    
    return true;
}

cc::Point FloorMapping::getStartPosition(RoomModel* model)
{
    return {
        //random.interval(0.0f, size.width - model->totalSize.x),
        //random.interval(0.0f, size.height - model->totalSize.y)
        size.width / 2,
        size.height /2
    };
}

FloorMapping::CrossingInfo& FloorMapping::selectCrossing()
{
    return random.select(crossingLeft);
}
void FloorMapping::removeCrossing(const CrossingInfo& info)
{
    for(auto it = crossingLeft.begin(); it != crossingLeft.end(); it++)
    {
        auto current = *it;
        if (info.refGateIndex == current.refGateIndex && info.refRoomIndex == current.refRoomIndex)
        {
            crossingLeft.erase(it);
            break;
        }
    }
}

std::list<cc::Point> FloorMapping::extractPositions(RoomModel* model,
                                      GateInfo gateAbsRef,
                                      GateInfo gateRelDest)
{
    std::list<cc::Point> result;
    
    if (gateAbsRef.type == gateRelDest.type)
    {
        auto el = this->gateConfig[gateAbsRef.type].second;
        cc::Size sub = {
            gateAbsRef.rect.size.width - el.size.width,
            gateAbsRef.rect.size.height - el.size.height,
        };
        cc::Rect rectFinal = {
            gateAbsRef.rect.origin.x - sub.width,
            gateAbsRef.rect.origin.y - sub.height,
            gateRelDest.rect.size.width + sub.width,
            gateRelDest.rect.size.height + sub.height
        };
        
        std::list<std::pair<cc::Point, cc::Rect>> data;
        cc::Rect bestSize = {0, 0, this->size.width, this->size.height};
        for(auto j = rectFinal.origin.y; j <= rectFinal.getMaxY(); j += model->tileSize.height)
            for(auto i = rectFinal.origin.x; i <= rectFinal.getMaxX(); i += model->tileSize.width)
            {
                cc::Point roomPos = {i - gateRelDest.rect.origin.x, j - gateRelDest.rect.origin.y};
                if (this->checkRoom(roomPos, model))
                {
                    auto candidateRect = cc::Rect(roomPos.x, roomPos.y,
                        model->totalSize.width, model->totalSize.height).unionWithRect(this->totalBounds);
                    if ((candidateRect.size.width + candidateRect.size.height) <=
                        (bestSize.size.width + bestSize.size.height))
                    {
                        bestSize = candidateRect;
                        data.push_back(std::make_pair(roomPos, candidateRect));
                    }
                }
            }
        
        for(auto pair : data)
        {
            if (pair.second.size.height + pair.second.size.width == bestSize.size.width + bestSize.size.height)
                result.push_back(pair.first);
        }
    }
    
    return result;
}

GateMap FloorMapping::createSpecialGate(RoomData* room, RoomData::RoomType roomType,
                          std::map<GateInfo::GateType, std::pair<std::string, cc::Rect>> gateConfig)
{
    CCAssert(room->crossAreas.size() > 0, "crossAreas empty");
    auto crossInfo = random.pick(room->crossAreas);
    
    GateMap gateMap;
    if (roomType == RoomData::RoomType::STARTER)
        gateMap.cmd = GateMap::CmdType::ENTER_MAP;
    else if (roomType == RoomData::RoomType::FINISHER)
        gateMap.cmd = GateMap::CmdType::EXIT_MAP;
    else
    {
        CCASSERT(false, "invalid room type");
    }
    
    auto tileSize = room->getBlockBound({0,0}).size;
    auto gateRect = gateConfig[crossInfo.type].second;
    
    int tx = (crossInfo.rect.size.width - gateRect.size.width) / tileSize.width;
    int ty = (crossInfo.rect.size.height - gateRect.size.height) / tileSize.height;
    
    gateMap.tileName = gateConfig[crossInfo.type].first;
    gateMap.info.type = crossInfo.type;
    gateMap.info.rect = cc::Rect(
                                 crossInfo.rect.origin.x + (tileSize.width * random.interval(0, tx)),
                                 crossInfo.rect.origin.y + (tileSize.height * random.interval(0, ty)),
                                 gateRect.size.width,
                                 gateRect.size.height);
    gateMap.destGateIndex = 0;
    gateMap.destRoomIndex = 0;
    
    return gateMap;
}

std::list<GateMap> FloorMapping::bindGates(cc::Point pos,
                                           RoomData* roomData,
                                           unsigned& gateIndex)
{
    std::list<GateMap> result;
    
    auto it1 = roomData->crossAreas.begin();
    cc::Size blockSize = roomData->getBlockBound({0,0}).size;
    
    while(it1 != roomData->crossAreas.end() && this->crossingLeft.size() > 0)
    {
        GateInfo crossArea = *it1;
        cc::Rect crossRect = {
            pos.x + crossArea.rect.origin.x,
            pos.y + crossArea.rect.origin.y,
            crossArea.rect.size.width,
            crossArea.rect.size.height
        };
        
        bool found = false;
        auto it2 = this->crossingLeft.begin();
        while(it2 != this->crossingLeft.end())
        {
            auto crossInfo = *it2;
            
            if (crossArea.type != crossInfo.gateInfo.type ||
                !crossRect.intersectsRect(crossInfo.gateInfo.rect))
            {
                it2++;
                continue;
            }
            
            auto intersect = lib::getIntersection(crossInfo.gateInfo.rect,
                                                  crossRect);
            auto gateRect = this->gateConfig[crossInfo.gateInfo.type].second;
            if (intersect.size.width >= gateRect.size.width &&
                intersect.size.height >= gateRect.size.height) //matching
            {
                //define dest Gate
                GateMap destGateMap;
                destGateMap.cmd = GateMap::CmdType::CHANGE_ROOM;
                destGateMap.tileName = this->gateConfig[crossInfo.gateInfo.type].first;
                int tx = (intersect.size.width - gateRect.size.width) / blockSize.width;
                int ty = (intersect.size.height - gateRect.size.height) / blockSize.height;
                destGateMap.info.type = crossInfo.gateInfo.type;
                destGateMap.info.rect = cc::Rect(
                    intersect.origin.x + (blockSize.width * random.interval(0, tx)) - pos.x,
                    intersect.origin.y + (blockSize.height * random.interval(0, ty)) - pos.y,
                    gateRect.size.width, gateRect.size.height);
                destGateMap.destRoomIndex = crossInfo.refRoomIndex;
                destGateMap.destGateIndex = crossInfo.refGateIndex;
                roomData->gateMapping[gateIndex] = destGateMap;
                
                //define srcGate
                GateMap srcGateMap;
                srcGateMap.cmd = GateMap::CmdType::CHANGE_ROOM;
                srcGateMap.info = this->getOppositeFromGateInfo(destGateMap.info);
                srcGateMap.info.rect.origin += pos;
                srcGateMap.tileName = this->gateConfig[srcGateMap.info.type].first;
                srcGateMap.destRoomIndex = roomData->index;
                srcGateMap.destGateIndex = gateIndex;
                result.push_back(srcGateMap);
                
                //updates
                it2 = this->crossingLeft.erase(it2);
                gateIndex++;
                found = true;
                break;
            }
            it2++;
        }
        
        if (found)
            it1 = roomData->crossAreas.erase(it1);
        else
            it1++;
    }
    
    return result;
}

GateInfo FloorMapping::getOppositeFromGateInfo(GateInfo info)
{
    cc::Rect rect = info.rect;
    switch(info.type)
    {
        case GateInfo::GateType::Down:
            rect.size.height = gateConfig[GateInfo::GateType::Up].second.size.height;
            rect.origin.y -= gateConfig[GateInfo::GateType::Up].second.size.height + margin;
            break;
        case GateInfo::GateType::Up:
            rect.size.height = gateConfig[GateInfo::GateType::Down].second.size.height;
            rect.origin.y += gateConfig[GateInfo::GateType::Up].second.size.height + margin;
            break;
        case GateInfo::GateType::Left:
            rect.size.width = gateConfig[GateInfo::GateType::Right].second.size.width;
            rect.origin.x -= gateConfig[GateInfo::GateType::Right].second.size.width + margin;
            break;
        case GateInfo::GateType::Right:
            rect.size.width = gateConfig[GateInfo::GateType::Left].second.size.width;
            rect.origin.x += gateConfig[GateInfo::GateType::Right].second.size.width + margin;
            break;
        default:
            CCAssert(false, "unhandled gate type");
            break;
    }
    
    return GateInfo
    {
        .type = info.getOpposite(),
        .rect = rect
    };
}

bool FloorMapping::withCrossingLeft()
{
    return crossingLeft.size() > 0;
}

bool FloorMapping::tryAddCross(cc::Point roomPos, GateInfo crossing, unsigned roomIndex, unsigned gateIndex)
{
    cc::Rect absRect = {
        crossing.rect.origin.x + roomPos.x, crossing.rect.origin.y + roomPos.y,
        crossing.rect.size.width, crossing.rect.size.height
    };
    crossing.rect = absRect;
    
    auto oppositeInfo = getOppositeFromGateInfo(crossing);
    
    for(auto rect : this->takenList)
    {
        if (oppositeInfo.rect.intersectsRect(rect))
            return false;
    }
    
    crossingLeft.push_back(CrossingInfo{
        .refRoomIndex = roomIndex,
        .refGateIndex = gateIndex,
        .gateInfo = oppositeInfo
    });
    
    return true;
}

bool FloorMapping::tryChangeCross(FloorMapping::CrossingInfo &crossingInfo,
                                  cc::Point roomPos,
                                  GateInfo newGateInfo)
{
    cc::Rect absRect = {
        newGateInfo.rect.origin.x + roomPos.x, newGateInfo.rect.origin.y + roomPos.y,
        newGateInfo.rect.size.width, newGateInfo.rect.size.height
    };
    newGateInfo.rect = absRect;
    auto oppositeInfo = getOppositeFromGateInfo(newGateInfo);
    
    for(auto rect : this->takenList)
    {
        if (oppositeInfo.rect.intersectsRect(rect))
            return false;
    }
    
    crossingInfo.gateInfo = oppositeInfo;
    
    return true;
}
