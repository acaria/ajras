#pragma once

#include "Random.h"
#include "TmxDataModel.h"
#include "IMapData.h"
#include "WarpMap.h"

class CampData : public TmxDataModel, public IMapData
{
public:
    static CampData* load(const std::string& fileName);

    CampData(const std::string& fileName);
    ~CampData();
    
    //getters
    const std::set<std::string>&    getSpriteSheets();
    lib::v2u                        getDim();
    cc::Rect                        getBounds();
    
    virtual lib::DataGrid<BlockInfo>& getGrid() override;
    virtual cc::Size getTileSize() override;
    virtual int getZOrder(const cc::Vec2& pos) override;
    virtual cc::Rect getBlockBound(lib::v2u coord) override;
    
    std::vector<WarpMap>            warpMapping;

private:
    lib::Random&                    random;
    std::set<std::string>           spriteSheets;
};