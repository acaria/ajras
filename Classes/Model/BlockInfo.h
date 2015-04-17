#pragma once

struct BlockInfo {
    enum PType {
        bgTileName,
        fgTileName,
        collision
    };

    std::map<PType, std::string>  fields;
};