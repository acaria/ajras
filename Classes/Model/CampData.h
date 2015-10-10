#pragma once

#include "Random.h"
#include "TmxDataModel.h"

class CampData : public TmxDataModel
{
public:
    static CampData* load(const std::string& fileName);

    CampData(const std::string& fileName);
    ~CampData();
    
    const std::set<std::string>&    getSpriteSheets();

private:
    lib::Random&                    random;
    std::set<std::string>           spriteSheets;
};