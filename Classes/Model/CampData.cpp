#include "CampData.h"
#include "Randgine.h"

CampData::CampData(const std::string& fileName) :
    random(Randgine::instance()->get(Randgine::CAMP)),
    TmxDataModel(fileName)
{
    //hack: only 1 ss per camp
    this->spriteSheets.insert(fileName);
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

cc::Size CampData::getTileSize()
{
    return {(float)this->tileSize.x, (float)this->tileSize.y};
}

int CampData::getZOrder(const cc::Vec2& pos)
{
    return this->getZOrder(pos);
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

lib::DataGrid<BlockInfo>& CampData::getContent()
{
    return this->grid;
}