#pragma once

#include "CoreLib.h"
#include "CollectibleData.h"

struct SlotData
{
    unsigned            quantity;
    CollectibleData*    content;
};

struct SlotCurrency
{
    unsigned            quantity;
    int                 stackability; //-1: unlimited
};