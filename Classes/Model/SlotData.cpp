#include "SlotData.h"

bool SlotData::checkFreeSlot(const std::list<SlotData> &inventory,
                             CollectibleData* element)
{
    for(auto slot : inventory)
    {
        if (slot.quantity == 0)
            return true;
        if (slot.content->key == element->key && slot.quantity < slot.content->stackability)
            return true;
    }
    return false;
}

bool SlotData::addCollectible(std::list<SlotData> &inventory,
                              CollectibleData* element)
{
    for(auto& slot : inventory)
    {
        if (slot.quantity == 0)
            continue;
        if (slot.content->key == element->key && slot.quantity < slot.content->stackability)
        {
            slot.quantity++;
            return true;
        }
    }
    for(auto& slot : inventory)
    {
        if (slot.quantity == 0)
        {
            slot.quantity = 1;
            slot.content = element;
            return true;
        }
    }
    return false;
}