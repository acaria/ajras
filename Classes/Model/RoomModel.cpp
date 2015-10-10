#include "RoomModel.h"
#include "Jsonxx.h"
#include "CoreLib.h"

RoomModel* RoomModel::create(const std::string &fileName)
{
    return new RoomModel(fileName);
}

void RoomModel::initialize()
{
    //todo: define walls for non rectangular rooms
    this->walls.push_back({0, 0,
        (float)this->totalSize.x, (float)this->totalSize.y});
    
    //processing cross areas
    for(unsigned j = 0; j < this->grid.height; j++)
    {
        for(unsigned i = 0; i < this->grid.width; i++)
        {
            auto &block = this->grid.get({i,j});
            if (block.fields.find(BlockInfo::collision) != block.fields.end())
            {
                if (block.fields[BlockInfo::collision] == "crossing")
                    this->crossAreas.push_back(extractCrossArea(*this, {i, j}));
            }
            if (block.fields.find(BlockInfo::sleepZone) != block.fields.end() &&
                block.fields.find(BlockInfo::sleepCat) != block.fields.end())
            {
                auto cat = AIComponent::mapSleep[block.fields[BlockInfo::sleepCat]];
                auto blockPos = this->getPosFromCoord({i,j});
                std::vector<std::string> coord;
                lib::split(block.fields[BlockInfo::sleepZone], coord, ",", true);
                this->sleepZones[cat].push_back(cc::Rect(
                    blockPos.x + std::stod(coord[0]),
                    blockPos.y + std::stod(coord[1]),
                    std::stod(coord[2]), std::stod(coord[3])
                ));
            }
        }
    }
}

GateInfo RoomModel::extractCrossArea(RoomModel& roomModel, lib::v2u coord)
{
    GateInfo gate {
        .rect = roomModel.getRectCoord({coord.x,coord.y}),
        .type = GateInfo::Unknowm
    };
    
    int maxK = roomModel.grid.width;
    int maxL = roomModel.grid.height;
    bool yAxis = false;
    for(unsigned l = coord.y; l < maxL; l++)
        for(unsigned k = coord.x; k < maxK; k++)
        {
            auto &block = roomModel.grid.get({k, l});
            if (block.fields.find(BlockInfo::collision) != block.fields.end() &&
                block.fields[BlockInfo::collision] == "crossing")
            {
                gate.rect = roomModel.getRectCoord({k,l}).unionWithRect(gate.rect);
                if (gate.type == GateInfo::Unknowm)
                    gate.type = gessGateType({k,l}, roomModel);
                block.fields.erase(BlockInfo::collision);// = "walkable";
            }
            else
            {
                if (yAxis)
                    maxL = l;
                else
                {
                    yAxis = true;
                    maxK = k;
                }
            }
        }
    
    CCASSERT(gate.type != GateInfo::Unknowm, "gate extraction error");
    return gate;
}

GateInfo::GateType RoomModel::gessGateType(lib::v2u pos, const RoomModel& roomModel)
{
    if (pos.x == 0)
        return GateInfo::Left;
    if (pos.y == 0)
        return GateInfo::Down;
    if (pos.x == roomModel.grid.width - 1)
        return GateInfo::Right;
    if (pos.y == roomModel.grid.height - 1)
        return GateInfo::Up;
    return GateInfo::Unknowm;
}