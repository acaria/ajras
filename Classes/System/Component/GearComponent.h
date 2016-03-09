#pragma once

#include "SlotData.h"

struct GearComponent
{
    GearComponent()
    {
        currency.quantity = 0;
        currency.stackability = 0;
    }
    
    SlotCurrency        currency;
    std::list<SlotData> slots;
};

