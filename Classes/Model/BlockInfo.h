#pragma once

struct BlockInfo {
    enum PType {
        bgTileName,
        fgTileName,
        collision,
        sleepZone,
        sleepCat,
        gating
    };

    std::map<PType, std::string>  fields;
};