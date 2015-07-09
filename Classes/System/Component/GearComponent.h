#pragma once

#include "SlotData.h"
#include "CollectibleData.h"

struct GearComponent
{
    void set(std::list<ColCat> list);

    std::list<SlotData> slots;
    
    bool checkFreeSlot(CollectibleData* data);
    void addCollectible(CollectibleData* data);
};

