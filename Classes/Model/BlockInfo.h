#pragma once
#include "Headers.h"

struct BlockInfo {
    enum PType {
        bgTileName,
        fgTileName,
        collision
    };

    std::map<PType, std::string>  fields;
};