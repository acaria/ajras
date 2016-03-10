#pragma once

#include "CoreLib.h"
#include "SlotData.h"
#include "TeamComponent.h"

struct PlayerEntity
{
    std::string                     profileName;
    unsigned                        entityID;
    unsigned                        ctrlIndex;
    std::list<SlotData>             inventory;
    SlotCurrency                    currency;
    TeamComponent                   team;
    float                           health;
    float                           stamina;
};
