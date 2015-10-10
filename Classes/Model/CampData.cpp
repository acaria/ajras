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

const std::set<std::string>& CampData::getSpriteSheets()
{
    return this->spriteSheets;
}