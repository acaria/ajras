#pragma once

#include "SlotData.h"
#include "CollectibleData.h"

struct GearComponent
{
    void set(const std::list<SlotData*>& list);

    std::list<SlotData*> slots;
    
    bool checkFreeSlot(const CollectibleData* data);
    void addCollectible(CollectibleData* data);
};

