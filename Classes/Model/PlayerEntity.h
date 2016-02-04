#pragma once

#include "CoreLib.h"
#include "SlotData.h"

struct PlayerEntity
{
    std::string             profileName;
    unsigned                entityID;
    unsigned                ctrlIndex;
    std::list<SlotData>     inventory;
    unsigned                health;
    unsigned                stamina;
};
