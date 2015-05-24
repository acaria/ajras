#pragma once

struct BlockInfo {
    enum PType {
        bgTileName,
        fgTileName,
        collision,
        sleepZone,
        sleepCat
    };

    std::map<PType, std::string>  fields;
};