#pragma once

#include "CoreLib.h"
#include "CollectibleData.h"

struct SlotData
{
    static bool checkFreeSlot(const std::list<SlotData>& inventory,
                              CollectibleData* element);
    static bool addCollectible(std::list<SlotData>& inventory,
                               CollectibleData* element);


    unsigned            quantity;
    CollectibleData*    content;
};

