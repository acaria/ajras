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
    
    this->init();
}

CampData::~CampData()
{
}

void CampData::init()
{
    //gates
    for(auto& gate : warpMapping)
    {
        cc::Point pos = {gate.info.rect.origin.x, gate.info.rect.origin.y};
        
        //change collision data
        
        auto gateSrcCoord = getCoordFromPos(pos);
        auto gateDestCoord = getCoordFromPos({
            gate.info.rect.getMaxX(),
            gate.info.rect.getMaxY()});
        
        for(int j = gateSrcCoord.y; j < gateDestCoord.y; j++)
        for(int i = gateSrcCoord.x; i < gateDestCoord.x; i++)
        {
            grid.get(i, j).fields[BlockInfo::collision] = "walkable";
        }
    }
    
    this->collision.init(this);
    this->navigation.init(this);
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

NavigationInfo* CampData::getNav()
{
    return &navigation;
}

cc::Size CampData::getTileSize()
{
    return {(float)this->tileSize.x, (float)this->tileSize.y};
}

cc::Rect CampData::getBlockBound(const lib::v2u& coord)
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

lib::v2u CampData::getCoordFromPos(const cc::Vec2& pos)
{
    return TmxDataModel::getCoordFromPos(pos);
}