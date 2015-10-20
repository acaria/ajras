#include "CampData.h"
#include "Randgine.h"

CampData::CampData(const std::string& fileName) :
    random(Randgine::instance()->get(Randgine::CAMP)),
    TmxDataModel(fileName)
{
    //hack: only 1 ss per camp
    this->spriteSheets.insert(fileName);
    
    for(auto obj : this->objs)
    {
        if (obj.type == "door" && lib::hasKey(obj.properties, "target"))
        {
            WarpMap warp = {
                .keyCmd = obj.properties["target"],
                .info = {
                    .rect = {obj.pos.x, obj.pos.y, obj.size.width, obj.size.height},
                    .type = GateInfo::GateType::Up
                }
            };
            this->warpMapping.push_back(warp);
        }
    }
    
    this->collision.init(this);
}

CampData::~CampData()
{
}

CampData* CampData::load(const std::string& filename)
{
    return new CampData(filename);
}

lib::DataGrid<BlockInfo>& CampData::getGrid()
{
    return this->grid;
}

CollisionInfo* CampData::getCol()
{
    return &collision;
}

cc::Size CampData::getTileSize()
{
    return {(float)this->tileSize.x, (float)this->tileSize.y};
}

int CampData::getZOrder(const cc::Vec2& pos)
{
    return TmxDataModel::getZOrder(pos);
}

cc::Rect CampData::getBlockBound(lib::v2u coord)
{
    return this->getRectCoord(coord);
}

const std::set<std::string>& CampData::getSpriteSheets()
{
    return this->spriteSheets;
}

lib::v2u CampData::getDim()
{
    return {grid.width, grid.height};
}

cc::Rect CampData::getBounds()
{
    return {
        0, 0, (float)totalSize.x, (float)totalSize.y
    };
}

lib::v2u CampData::getCoordFromPos(cocos2d::Vec2 pos)
{
    return TmxDataModel::getCoordFromPos(pos);
}