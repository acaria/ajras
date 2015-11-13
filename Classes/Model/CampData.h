#pragma once

#include "Random.h"
#include "TmxDataModel.h"
#include "IMapData.h"
#include "WarpMap.h"
#include "CollisionInfo.h"

class CampData : public TmxDataModel, public IMapData
{
public:
    static CampData* load(const std::string& fileName);

    CampData(const std::string& fileName);
    ~CampData();
    
    //getters
    const std::set<std::string>&    getSpriteSheets();
    lib::v2u                        getDim();
    
    //overrides
    cc::Rect                        getBounds() override;
    virtual lib::DataGrid<BlockInfo>& getGrid() override;
    virtual cc::Size getTileSize() override;
    virtual cc::Rect getBlockBound(const lib::v2u& coord) override;
    virtual lib::v2u getCoordFromPos(const cocos2d::Vec2& pos) override;
    virtual CollisionInfo* getCol() override;
    
    std::vector<WarpMap>            warpMapping;

private:
    CollisionInfo                   collision;
    lib::Random&                    random;
    std::set<std::string>           spriteSheets;
};